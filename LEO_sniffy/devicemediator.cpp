#include "devicemediator.h"

DeviceMediator::DeviceMediator(QObject *parent) : QObject(parent)
{
    communication = new Comms();

    connect(communication,SIGNAL(devicesScaned(QList<DeviceDescriptor>)),this,SLOT(newDeviceList(QList<DeviceDescriptor>)),Qt::QueuedConnection);

    modules = createModulesList();

    connect(device,&Device::ScanDevices,this,&DeviceMediator::ScanDevices);
    connect(device,&Device::openDevice,this,&DeviceMediator::open);
    connect(device,&Device::closeDevice,this,&DeviceMediator::close);
}

QList<QSharedPointer<AbstractModule>> DeviceMediator::createModulesList(){
    QList<QSharedPointer<AbstractModule>> tmpModules;
    tmpModules.append(QSharedPointer<AbstractModule> (device = new Device(this)));
    tmpModules.append(QSharedPointer<AbstractModule> (new Scope(this)));
    tmpModules.append(QSharedPointer<AbstractModule> (new Counter(this)));
    tmpModules.append(QSharedPointer<AbstractModule> (new Voltmeter(this)));
    tmpModules.append(QSharedPointer<AbstractModule> (new TemplateModule(this)));

    foreach(QSharedPointer<AbstractModule> mod, tmpModules){
        connect(mod.data(), &AbstractModule::blockConflictingModules, this, &DeviceMediator::blockConflictingModulesCallback);
        connect(mod.data(), &AbstractModule::releaseConflictingModules, this, &DeviceMediator::releaseConflictingModulesCallback);
    }

    return tmpModules;
}

QList<QSharedPointer<AbstractModule>> DeviceMediator::getModulesList(){
    return modules;
}

void DeviceMediator::ScanDevices(){
    communication->scanForDevices();
}

void DeviceMediator::newDeviceList(QList<DeviceDescriptor> deviceList){
    this->deviceList = deviceList;
    //qDebug() << "scanned device list received in device.cpp";
    device->updateGUIDeviceList(deviceList);
}

void DeviceMediator::open(int deviceIndex){
    communication->open(deviceList.at(deviceIndex));

    QThread::msleep(50);
    int i = 0;
    while (communication->getIsOpen()==false) {
        QThread::msleep(500);
        i++;
        if(i>5){
            device->errorHandler("Device cannot be opened");
            break;
        }
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();

    if(isConnected){
        connect(communication,&Comms::newData,this,&DeviceMediator::parseData);
        connect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);

        foreach(QSharedPointer<AbstractModule> mod, modules){
            mod->setComms(communication);
        }
        emit loadLayout(deviceList.at(deviceIndex).deviceName);
    }
}

void DeviceMediator::blockConflictingModulesCallback(QString moduleName, int resources)
{
    if(resourcesInUse & resources){
        qDebug () << "FATAL ERROR - trying to open conflicting resources";
    }
    foreach(QSharedPointer<AbstractModule> mod, modules){
        if(mod->getResources() & resources && mod->getModuleName()!=moduleName)
            mod->setModuleStatus(ModuleStatus::LOCKED);
    }
    resourcesInUse = resourcesInUse | resources;
}

void DeviceMediator::releaseConflictingModulesCallback(QString moduleName, int resources)
{
    foreach(QSharedPointer<AbstractModule> mod, modules){
        if(mod->getResources() & resources && mod->getModuleName()!=moduleName)
            mod->setModuleStatus(ModuleStatus::STOP);
    }
    resourcesInUse = (resourcesInUse ^ resources) & resourcesInUse;
}

void DeviceMediator::close(){
    if(isConnected){
        foreach(QSharedPointer<AbstractModule> mod, modules){
            mod->disableModule();
        }
        communication->close();
        isConnected = false;
    }
    ShowDeviceModule();

    disconnect(communication,&Comms::newData,this,&DeviceMediator::parseData);
    disconnect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);
}

void DeviceMediator::handleError(QByteArray error){
    device->errorHandler(error);
}

void DeviceMediator::parseData(QByteArray data){
    bool isDataPassed = false;
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    //What if data belongs to voltmeter and scope???????????
    //Solution1: each module has to know if it is running or not and handle data correctly.
    foreach(QSharedPointer<AbstractModule> module, modules){
        if(dataHeader == module->getCommandPrefix()){
            module->parseData(dataToPass);
            isDataPassed=true;
        }
    }
    if(!isDataPassed){
        qDebug() << "ERROR: this data was not passed to any module" << data;
    }
}

void DeviceMediator::ShowDeviceModule(){
    device->showModuleWindow();
    device->showModuleControl();
    device->hideModuleStatus();
}

bool DeviceMediator::getIsConnected() const
{
    return isConnected;
}

QString DeviceMediator::getDeviceName(){
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

