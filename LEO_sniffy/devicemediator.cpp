#include "devicemediator.h"
#include <QTimer>

DeviceMediator::DeviceMediator(QObject *parent) : QObject(parent)
{
    communication = new Comms();

    connect(communication, SIGNAL(devicesScaned(QList<DeviceDescriptor>)), this, SLOT(newDeviceList(QList<DeviceDescriptor>)), Qt::QueuedConnection);
    modules = createModulesList();

    connect(device, &Device::ScanDevices, this, &DeviceMediator::ScanDevices);
    connect(device, &Device::openDevice, this, &DeviceMediator::open);
    connect(device, &Device::closeDevice, this, &DeviceMediator::close);

    // initialize ResourceManager aggregates
    resourceManager.reset();
}

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

void DeviceMediator::open(int deviceIndex)
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

        QString layoutFile;
        QString configFile;
        const QString devName = deviceList.at(deviceIndex).deviceName;
        if (devName.isEmpty())
        {
            CustomSettings::setNoSessionfound();
        }
        else
        {
            layoutFile = QApplication::applicationDirPath() + "/sessions/" + devName + ".lay";
            configFile = QApplication::applicationDirPath() + "/sessions/" + devName + ".cfg";
            QSharedPointer<AbstractModule> module;

            QFile file(layoutFile);
            QFile fileMod(configFile);

            if (file.exists() && fileMod.exists())
            {
                CustomSettings::askForSessionRestore(devName);
            }
            else
            {
                CustomSettings::setNoSessionfound();
            }
        }

        // send and valiadate token
        if (CustomSettings::getLoginToken()!="none"){
            communication->write("SYST:MAIL:" + CustomSettings::getUserEmail().toUtf8() + ";");
            communication->write("SYST:TIME:" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toUtf8() + ";");
            communication->write("SYST:PIN_:" + CustomSettings::getUserPin().toUtf8() + ";");
            communication->write("TKN_:TIME:" + CustomSettings::getTokenValidity().toString("yyyy-MM-dd HH:mm:ss").toUtf8() + ";");
            communication->write("TKN_:DATA:" + QByteArray::fromHex(CustomSettings::getLoginToken()) + ";");
        }
        for (const QSharedPointer<AbstractModule> &mod : modules)
        {
            mod->setComms(communication);
        }
        emit loadLayout(deviceList.at(deviceIndex).deviceName);
    }
}

void DeviceMediator::reopenDeviceAfterLogin()
{
    // Called when the login info changed. If we were connected to a device,
    // briefly close and reopen so the MCU receives the new token handshake.
    if (currentDeviceIndex < 0 || deviceList.isEmpty()) return;

    // If not connected, nothing to do — open directly
    if (!isConnected) {
        open(currentDeviceIndex);
        return;
    }
    close();

    QTimer::singleShot(250, [this]() {
        if (currentDeviceIndex >= 0 && currentDeviceIndex < deviceList.size()) {
            open(currentDeviceIndex);
        }
    });
}

void DeviceMediator::disableModules()
{
    if (isConnected)
    {
        emit saveLayout();
        for (const QSharedPointer<AbstractModule> &mod : modules)
        {
            mod->disableModule();
        }
        communication->close();
        isConnected = false;
    }
}

void DeviceMediator::blockConflictingModulesCallback(QString moduleName, int resources)
{
    if (resourcesInUse & resources)
    {
        qDebug() << "FATAL ERROR - trying to open conflicting resources";
    }

    // Find the module that is starting
    QSharedPointer<AbstractModule> starter;
    for (const QSharedPointer<AbstractModule> &m : modules)
    {
        if (m->getModuleName() == moduleName)
        {
            starter = m;
            break;
        }
    }

    // Prepare aggregate set including the starter's masks
    ResourceSet starterSet;
    if (starter) {
        starterSet.resources = resources;
        starterSet.gpioA = starter->getGpioMaskA();
        starterSet.gpioB = starter->getGpioMaskB();
        starterSet.gpioC = starter->getGpioMaskC();
        starterSet.gpioD = starter->getGpioMaskD();
    } else {
        starterSet.resources = resources;
    }

    for (const QSharedPointer<AbstractModule> &mod : modules)
    {
        if (mod->getModuleName() == moduleName) continue;

        // ResourceSet for the examined module
        ResourceSet set;
        set.resources = mod->getResources();
        set.gpioA = mod->getGpioMaskA();
        set.gpioB = mod->getGpioMaskB();
        set.gpioC = mod->getGpioMaskC();
        set.gpioD = mod->getGpioMaskD();

        bool lock = ResourceSet::intersects(set, starterSet);

        if (lock)
            mod->setModuleStatus(ModuleStatus::LOCKED);
    }
    // Reserve legacy resources and GPIO masks for the starting module
    resourcesInUse = resourcesInUse | resources;
    resourceManager.reserve(starterSet);
}

void DeviceMediator::releaseConflictingModulesCallback(QString moduleName, int resources)
{
    resourcesInUse = (resourcesInUse ^ resources) & resourcesInUse;

    // release masks of the stopping module
    for (const QSharedPointer<AbstractModule> &m : modules) {
        if (m->getModuleName() == moduleName) {
            ResourceSet set;
            set.resources = resources;
            set.gpioA = m->getGpioMaskA();
            set.gpioB = m->getGpioMaskB();
            set.gpioC = m->getGpioMaskC();
            set.gpioD = m->getGpioMaskD();
            resourceManager.release(set);
            break;
        }
    }

    for (const QSharedPointer<AbstractModule> &mod : modules)
    {
        if (mod->getModuleName() == moduleName) continue;

            // Legacy resources condition stays
            bool canUnlock = (((mod->getResources() ^ resources) & resourcesInUse) == 0);

            // Additionally ensure no GPIO conflict with any currently reserved mask
            ResourceSet modRes = resourceManager.reserved();
            ResourceSet modSet;
            modSet.resources = mod->getResources();
            modSet.gpioA = mod->getGpioMaskA();
            modSet.gpioB = mod->getGpioMaskB();
            modSet.gpioC = mod->getGpioMaskC();
            modSet.gpioD = mod->getGpioMaskD();
            canUnlock = canUnlock && !ResourceSet::intersects(modSet, modRes);

        if (canUnlock)
            mod->setModuleStatus(ModuleStatus::STOP);
    }
}

void DeviceMediator::close()
{
    disconnect(communication, &Comms::newData, this, &DeviceMediator::parseData);
    disconnect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);
    disableModules();
    ShowDeviceModule();
}

void DeviceMediator::closeApp()
{
    disconnect(communication, &Comms::newData, this, &DeviceMediator::parseData);
    disconnect(communication, &Comms::communicationError, this, &DeviceMediator::handleError);
    disableModules();

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
        if (dataHeader == module->getCommandPrefix() && (module->isActive() || dataToPass.left(4) == "CFG_" || dataToPass.left(4) == "ACK_"))
        {
            module->parseData(dataToPass);
            isDataPassed = true;
        }
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

int DeviceMediator::getResourcesInUse() const
{
    return resourcesInUse;
}

void DeviceMediator::setResourcesInUse(int value)
{
    resourcesInUse = value;
}
