#include "devicemediator.h"
#include "resourcemanager.h"
#include <QTimer>
#include <QThread>
#include "authenticator.h"

DeviceMediator::DeviceMediator(Authenticator *auth, QObject *parent) 
    : QObject(parent), authenticator(auth)
{
    communication = new Comms();

    connect(communication, SIGNAL(devicesScaned(QList<DeviceDescriptor>)), this, SLOT(newDeviceList(QList<DeviceDescriptor>)), Qt::QueuedConnection);
    modules = createModulesList();

    connect(device, &Device::ScanDevices, this, &DeviceMediator::ScanDevices);
    connect(device, &Device::openDevice, this, &DeviceMediator::openDevice);
    connect(device, &Device::closeDevice, this, &DeviceMediator::close);
    connect(device, &Device::deviceSpecificationReady, this, &DeviceMediator::onDeviceSpecificationReady);

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
    communication->scanForDevices();
}

void DeviceMediator::newDeviceList(QList<DeviceDescriptor> deviceList)
{
    this->deviceList = deviceList;
    // qDebug() << "scanned device list received in device.cpp";
    device->updateGUIDeviceList(deviceList);
}

void DeviceMediator::openDevice(int deviceIndex)
{
    // Remember which index is currently opened so we can reopen after login
    currentDeviceIndex = deviceIndex;
    communication->open(deviceList.at(deviceIndex));

    QThread::msleep(50);
    int i = 0;
    while (communication->getIsOpen() == false)
    {
        QThread::msleep(500);
        i++;
        if (i > 5)
        {
            device->errorHandler("Device cannot be opened");
            break;
        }
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();

    if (isConnected)
    {
        connect(communication, &Comms::newData, this, &DeviceMediator::parseData);
        connect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);

        const QString devName = deviceList.at(deviceIndex).deviceName;

        // Clear previous right-side specifications before we start receiving CFG_/ACK_ again
        device->clearAllModuleDescriptions();

        // Immediately request MCU reset so it starts in known state. While we wait 250ms
        // the layout/config files can be opened and processed. After 250ms send the
        // login token (if present), attach modules to the comms and load the layout.
        communication->write(Commands::RESET_DEVICE+";");

        // Delay subsequent setup (token handshake, module wiring, layout load)
        QTimer::singleShot(250, [this, deviceIndex, devName]() {
            // send and validate token
            if (CustomSettings::getLoginToken() != "none"){
                communication->write("SYST:MAIL:" + CustomSettings::getUserEmail().toUtf8() + ";");
                communication->write("SYST:TIME:" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toUtf8() + ";");
                communication->write("SYST:PIN_:" + CustomSettings::getUserPin().toUtf8() + ";");
                communication->write("TKN_:TIME:" + CustomSettings::getTokenValidity().toString("yyyy-MM-dd HH:mm:ss").toUtf8() + ";");
                communication->write("TKN_:DATA:" + QByteArray::fromHex(CustomSettings::getLoginToken()) + ";");
            }

            // Check for session file before setting comms, so layout can be loaded
            // and modules can access JSON data when they show their controls
            if (devName.isEmpty())
            {
                CustomSettings::setNoSessionfound();
            }
            else
            {
                QString sessionFile = QApplication::applicationDirPath() + "/sessions/" + devName + ".json";
                QFile file(sessionFile);
                if (file.exists())
                {
                    CustomSettings::askForSessionRestore(devName);
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
        });
    }
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

    QTimer::singleShot(250, [this]() {
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
    disconnect(communication, &Comms::newData, this, &DeviceMediator::parseData);
    disconnect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);
    disconnectDevice();
    ShowDeviceModule();
}

void DeviceMediator::closeApp()
{
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
        if (dataHeader == module->getCommandPrefix() && (module->isActive() || dataToPass.left(4) == Commands::CONFIG || dataToPass.left(4) == Commands::ACK))
        {
            module->parseData(dataToPass);
            isDataPassed = true;
        }
    }
    if(dataHeader == Commands::ERROR){
        qDebug() << "DEVICE ERROR " << dataToPass.toHex();
        isDataPassed = true;
    }
    if(dataHeader == Commands::DEBUG){
        qDebug() << "DEVICE DEBUG " << dataToPass;
        isDataPassed = true;
    }
    if (!isDataPassed)
    {
        if (data.right(1) == "E")
        {
            qDebug() << "DEVICE ERROR " << QString::number((uint8_t)((data.right(2)).at(0)));
        }
        else if (data.left(3) == "DBG")
        {
            qDebug() << "DEBUG MCU: " << data.right(data.length() - 4);
        }
        else
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

void DeviceMediator::onDeviceSpecificationReady()
{
    // Trigger async re-auth with Device_name and MCU_ID after device specifications are parsed
    if (authenticator) {
        authenticator->tokenRefresh(device->getName(), device->getMcuId());
    }
}
