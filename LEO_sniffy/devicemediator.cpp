#include "devicemediator.h"
#include "resourcemanager.h"
#include <QTimer>
#include <QThread>
#include <QStandardPaths>
#include "authenticator.h"

DeviceMediator::DeviceMediator(Authenticator *auth, QObject *parent) 
    : QObject(parent), authenticator(auth)
{
    communication = new Comms();

    connect(communication, SIGNAL(devicesScaned(QList<DeviceDescriptor>)), this, SLOT(newDeviceList(QList<DeviceDescriptor>)), Qt::QueuedConnection);
    connect(communication, &Comms::connectionOpened, this, &DeviceMediator::onConnectionOpened);
    modules = createModulesList();

    connect(device, &Device::ScanDevices, this, &DeviceMediator::ScanDevices);
    connect(device, &Device::openDevice, this, &DeviceMediator::openDevice);
    connect(device, &Device::closeDevice, this, &DeviceMediator::close);
    connect(device, &Device::deviceSpecificationReady, this, &DeviceMediator::onDeviceSpecificationReady);

    // Safety timer: if FW never sends TKN_ACK, proceed after timeout
    tokenAckTimer = new QTimer(this);
    tokenAckTimer->setSingleShot(true);
    tokenAckTimer->setInterval(3000); // 3 seconds should be more than enough for flash operations
    connect(tokenAckTimer, &QTimer::timeout, this, &DeviceMediator::onTokenAckTimeout);

    // initialize ResourceManager aggregates
    resourceManager.reset();
}

DeviceMediator::~DeviceMediator() {}

QList<QSharedPointer<AbstractModule>> DeviceMediator::createModulesList()
{
    QList<QSharedPointer<AbstractModule>> tmpModules;
    tmpModules.append(QSharedPointer<AbstractModule>(device = new Device(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new Scope(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new Counter(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new Voltmeter(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new SyncPwm(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new ArbGenerator(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new ArbGenerator(this, true)));
    tmpModules.append(QSharedPointer<AbstractModule>(new PatternGenerator(this)));
    tmpModules.append(QSharedPointer<AbstractModule>(new VoltageSource(this)));
    //   tmpModules.append(QSharedPointer<AbstractModule> (new TemplateModule(this)));

    for (const QSharedPointer<AbstractModule> &mod : tmpModules)
    {
        connect(mod.data(), &AbstractModule::blockConflictingModules, this, &DeviceMediator::blockConflictingModulesCallback);
        connect(mod.data(), &AbstractModule::releaseConflictingModules, this, &DeviceMediator::releaseConflictingModulesCallback);
        connect(mod.data(), &AbstractModule::moduleDescription, device, &Device::addModuleDescription);
    }

    return tmpModules;
}

QList<QSharedPointer<AbstractModule>> DeviceMediator::getModulesList()
{
    return modules;
}

void DeviceMediator::ScanDevices()
{
    // Explicit Scan button press: never auto-connect.
    // The user wants to see the list and choose. Auto-connect is only
    // for the initial background scan at startup (default true).
    autoConnectOnSingleDevice = false;
    communication->scanForDevices();
}

void DeviceMediator::newDeviceList(QList<DeviceDescriptor> deviceList)
{
    // Ignore scan results while a device is connected (or mid-auth handshake).
    if (isConnected || waitingForTokenAck)
        return;

    this->deviceList = deviceList;
    device->updateGUIDeviceList(deviceList, autoConnectOnSingleDevice);
}

void DeviceMediator::openDevice(int deviceIndex)
{
    // Remember which index is currently opened so we can reopen after login
    currentDeviceIndex = deviceIndex;
    communication->open(deviceList.at(deviceIndex));
}

void DeviceMediator::onConnectionOpened(bool success)
{
    isConnected = success;
    isDemoMode = false;
    if (!isConnected)
    {
        device->errorHandler("Device cannot be opened");
        qDebug() << "ERROR wait for comm to be opened";
        return;
    }

    connect(communication, &Comms::newData, this, &DeviceMediator::parseData);
    connect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);

    const QString devName = deviceList.at(currentDeviceIndex).deviceName;

    // Clear previous right-side specifications before we start receiving CFG_/ACK_ again
    device->clearAllModuleDescriptions();

    // Immediately request MCU reset so it starts in known state.
    // USB-CDC re-enumeration takes ~100ms, give it 150ms total.
    communication->write(Commands::RESET_DEVICE+";");

    // Delay subsequent setup (token handshake, module wiring, layout load)
    QTimer::singleShot(150, [this, devName]() {
        // send and validate token
        if (CustomSettings::getLoginToken() != "none")
        {
            communication->write("SYST:MAIL:" + CustomSettings::getUserEmail().toUtf8() + ";");
            communication->write("SYST:TIME:" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toUtf8() + ";");

            QByteArray token = CustomSettings::getLoginToken();
            // If token is hex string (256 chars), convert to bytes.
            // If it's already bytes (128 chars from @ByteArray), use as is.
            if (token.size() == 256) {
                token = QByteArray::fromHex(token);
            }

            communication->write("TKN_:DATA:" + token + ";");

            // Wait for the FW to send TKN_ACK (or DEMO) before wiring modules.
            waitingForTokenAck = true;
            pendingDevName = devName;
            pendingDeviceIndex = currentDeviceIndex;
            tokenAckTimer->start(); // safety timeout
            qDebug() << "[Auth] Token sent, waiting for FW ACK before loading modules...";
        } else {
            finalizeDeviceOpen(currentDeviceIndex, devName);
        }
    });
}

void DeviceMediator::reopenDeviceAfterLogin()
{
    // Called when the login info changed. If we were connected to a device,
    // briefly close and reopen so the MCU receives the new token handshake.
    if (currentDeviceIndex < 0 || deviceList.isEmpty()) return;

    // If not connected, nothing to do — open directly
    if (!isConnected) {
        openDevice(currentDeviceIndex);
        return;
    }
    close();
    // Delay re-open so the serial thread has time to actually close the port.
    // Without this delay, openDevice() would try to open the same COM port
    // while the old handle is still being torn down in the serial thread.
    QTimer::singleShot(300, this, [this]() {
        if (currentDeviceIndex >= 0 && currentDeviceIndex < deviceList.size()) {
            openDevice(currentDeviceIndex);
        }
    });
}

void DeviceMediator::disableModules()
{
    for (const QSharedPointer<AbstractModule> &mod : modules)
    {
        mod->disableModule();
    }
}

void DeviceMediator::closeModules()
{
    for (const QSharedPointer<AbstractModule> &mod : modules)
    {
        mod->closeModule();
    }
}



void DeviceMediator::disconnectDevice()
{
    if (isConnected)
    {
        emit saveLayoutUponExit();
        disableModules();
        communication->close();
        isConnected = false;
    }
}

void DeviceMediator::blockConflictingModulesCallback(QString moduleName, int resources)
{
    // Find the module that is starting
    QSharedPointer<AbstractModule> starter;
    for (const QSharedPointer<AbstractModule> &m : modules) {
        if (m->getModuleName() == moduleName) {
            starter = m;
            break;
        }
    }
    // Prepare aggregate set including the starter's masks
    ResourceSet starterSet = ResourceSet::fromModule(starter, resources);
    for (const QSharedPointer<AbstractModule> &mod : modules) {
        if (mod->getModuleName() == moduleName)
            continue;
        ResourceSet set = ResourceSet::fromModule(mod);
        bool lock = ResourceSet::collide(set, starterSet);
        if (lock)
            mod->setModuleStatus(ModuleStatus::LOCKED);
    }
    resourceManager.reserve(starterSet);
}

void DeviceMediator::releaseConflictingModulesCallback(QString moduleName, int resources)
{
    // release masks of the stopping module
    for (const QSharedPointer<AbstractModule> &m : modules) {
        if (m->getModuleName() == moduleName) {
            ResourceSet set = ResourceSet::fromModule(m, resources);
            resourceManager.release(set);
            break;
        }
    }
    // Check all other modules if they can be unlocked now
    for (const QSharedPointer<AbstractModule> &mod : modules) {
        // Skip the module that is being released
        if (mod->getModuleName() == moduleName)
            continue;
        if (mod->getModuleStatus() == ModuleStatus::LOCKED) {
            ResourceSet reservedNow = resourceManager.reserved();
            ResourceSet modSet = ResourceSet::fromModule(mod, mod->getResources());
            const bool stillConflicts = ResourceSet::collide(modSet, reservedNow);
            if (!stillConflicts) {
                // Transition back to STOP to indicate "unlocked"; previous code used STOP for unlock state
                mod->setModuleStatus(ModuleStatus::STOP);
            }
        }
    }
}

void DeviceMediator::close()
{
    // Cancel any pending token ACK wait
    waitingForTokenAck = false;
    tokenAckTimer->stop();

    // Suppress auto-connect after manual disconnect so the background scanner
    // doesn't immediately reconnect to the device the user just left.
    autoConnectOnSingleDevice = false;

    disconnect(communication, &Comms::newData, this, &DeviceMediator::parseData);
    disconnect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);
    disconnectDevice();
    ShowDeviceModule();
}

void DeviceMediator::closeApp()
{
    waitingForTokenAck = false;
    tokenAckTimer->stop();

    disconnect(communication, &Comms::newData, this, &DeviceMediator::parseData);
    disconnect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);
    disconnectDevice();

}

void DeviceMediator::handleError(QByteArray error)
{
    device->errorHandler(error);
}

void DeviceMediator::parseData(QByteArray data)
{
    bool isDataPassed = false;
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length() - 4);

    for (const QSharedPointer<AbstractModule> &module : modules)
    {
        if (dataHeader == module->getCommandPrefix() && (module->isActive() || dataToPass.left(4) == Commands::CONFIG || dataToPass.left(4) == Commands::ACK || dataToPass.left(4) == Commands::INACTIVE))
        {
            module->parseData(dataToPass);
            isDataPassed = true;
            if(dataToPass.left(4) == Commands::INACTIVE){
                qDebug() << "Module" << module->getModuleName() << "is inactive.";
                module->closeModule();
            }
            if(dataToPass.left(4) == Commands::CONFIG){
                module->setModuleConfigured();
            }
        }
    }
    if(dataHeader == Commands::TOKEN && dataToPass.left(4) == Commands::ACK){
        isDataPassed = true;
        if (dataToPass.contains("DEMO")) {
            emit popupMessageRequested("Running in demo mode");
            isDemoMode = true;
        } else {
            isDemoMode = false;
        }

        // Deterministic auth: FW has finished all flash operations and verified
        // the token. NOW it's safe to wire modules — they will get correct
        // auth-required responses instead of IACT.
        if (waitingForTokenAck) {
            waitingForTokenAck = false;
            tokenAckTimer->stop();
            qDebug() << "[Auth] FW ACK received, proceeding to load modules (demo=" << isDemoMode << ")";
            finalizeDeviceOpen(pendingDeviceIndex, pendingDevName);
        }
    }
    if(dataHeader == Commands::ERROR){
        qDebug() << "ERROR " << dataToPass.toHex();
        emit popupMessageRequested("Device Error: " + QString::fromUtf8(dataToPass.toHex()));
        isDataPassed = true;

        // If we were waiting for token ACK and got an error instead, the auth
        // failed on the FW side. Proceed with module loading anyway — modules
        // requiring auth will simply be inactive (demo/limited mode).
        if (waitingForTokenAck) {
            waitingForTokenAck = false;
            tokenAckTimer->stop();
            qWarning() << "[Auth] FW returned error during token verification — loading modules without auth";
            finalizeDeviceOpen(pendingDeviceIndex, pendingDevName);
        }
    }
    if(dataHeader == Commands::DEBUG){
        qDebug() << "DEVICE DEBUG " << dataToPass;
        isDataPassed = true;
    }
    if (!isDataPassed)
    {
        if (data.length() < 30)
        {
            qDebug() << "ERROR: this data was not passed to any module" << data;
        }
        else
        {
            qDebug() << "ERROR: this data was not passed to any module" << data.left(15) << " ... " << data.right(10);
        }       
    }
}

void DeviceMediator::finalizeDeviceOpen(int deviceIndex, QString devName)
{
    // Check for session file before setting comms, so layout can be loaded
    // and modules can access JSON data when they show their controls
    if (devName.isEmpty())
    {
        CustomSettings::setNoSessionfound();
    }
    else
    {
        QString sessionFile = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/sessions/" + devName + ".json";
        QFile file(sessionFile);
        if (file.exists())
        {
            CustomSettings::askForSessionRestore(devName, qobject_cast<QWidget*>(this->parent()));
        }
        else
        {
            CustomSettings::setNoSessionfound();
        }
    }

    // Emit loadLayoutUponOpen BEFORE setComms so MainWindow can load the session
    // data (pendingSessionData) before modules start showing their controls
    if (CustomSettings::isSessionRestoreRequest() && deviceIndex >= 0 && deviceIndex < deviceList.size()) {
        emit loadLayoutUponOpen(deviceList.at(deviceIndex).deviceName);
    } else {
        // No session to restore, load default layout instead
        emit loadLayoutUponOpen("layoutOnly");
    }

    // Now attach modules to comms - they will call showModuleControl which triggers
    // deferred configuration restoration from the already-loaded session data
    for (const QSharedPointer<AbstractModule> &mod : modules)
    {
        mod->setComms(communication);
    }
}

void DeviceMediator::ShowDeviceModule()
{
    device->showModuleWindow();
    device->showModuleControl();
    device->hideModuleStatus();
}

bool DeviceMediator::getIsConnected() const
{
    return isConnected;
}

bool DeviceMediator::getIsDemoMode() const
{
    return isDemoMode;
}

QString DeviceMediator::getDeviceName()
{
    return device->getName();
}

QString DeviceMediator::getMcuId()
{
    return device->getMcuId();
}

ResourceSet DeviceMediator::getResourcesInUse() const
{
    return resourceManager.reserved();
}

void DeviceMediator::setResourcesInUse(ResourceSet resources)
{
    resourceManager.reserve(resources);
}

void DeviceMediator::onTokenAckTimeout()
{
    if (!waitingForTokenAck) return; // already handled
    waitingForTokenAck = false;
    qWarning() << "[Auth] FW did not send TKN_ACK within timeout — proceeding with module load anyway";
    finalizeDeviceOpen(pendingDeviceIndex, pendingDevName);
}

void DeviceMediator::onDeviceSpecificationReady()
{
    // Trigger async token refresh with Device_name and MCU_ID.
    // IMPORTANT: This refresh is automatic (background). It must NEVER force a
    // device reconnection — only manual logins through LoginDialog should reopen.
    // The refresh just extends the session silently; the new token will be used
    // on the next device open.
    if (authenticator) {
        if (lastTokenRefresh.isValid() && lastTokenRefresh.secsTo(QDateTime::currentDateTime()) < 30) {
            qDebug() << "Skipping automatic token refresh (last refresh < 30s ago)";
            return;
        }
        lastTokenRefresh = QDateTime::currentDateTime();
        authenticator->tokenRefresh(device->getName(), device->getMcuId(), isDemoMode);
    }
}
