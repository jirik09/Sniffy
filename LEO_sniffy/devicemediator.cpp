#include "devicemediator.h"

DeviceMediator::DeviceMediator(QObject *parent) : QObject(parent)
{
    communication = new Comms();

    connect(communication,SIGNAL(devicesScaned(QList<DeviceDescriptor>)),this,SLOT(newDeviceList(QList<DeviceDescriptor>)),Qt::QueuedConnection);

    modules = createModulesList();

    connect(device,&Device::ScanDevices,this,&DeviceMediator::ScanDevices);
    connect(device,&Device::opened,this,&DeviceMediator::open);
    connect(device,&Device::closed,this,&DeviceMediator::close);
}

QList<QSharedPointer<AbstractModule>> DeviceMediator::createModulesList(){
    QList<QSharedPointer<AbstractModule>> modules;
    modules.append(QSharedPointer<AbstractModule> (device = new Device(this)));
    modules.append(QSharedPointer<AbstractModule> (new Scope(this)));
    modules.append(QSharedPointer<AbstractModule> (new Counter(this)));
    return modules;
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

    while (communication->getIsOpen()==false) {
        QThread::msleep(500);
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();

    connect(communication,&Comms::newData,this,&DeviceMediator::parseData);
    connect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);

    communication->write(Commands::SYSTEM+":"+Commands::CONFIG_REQUEST+";");
    foreach(QSharedPointer<AbstractModule> mod, modules){
        mod->setComms(communication);
    }
}

void DeviceMediator::close(){
    if(isConnected){
        communication->close();
        foreach(QSharedPointer<AbstractModule> mod, modules){
            mod->disableModule();
        }
        isConnected = communication->getIsOpen();
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

