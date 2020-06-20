#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{
    deviceSpec = new DeviceSpec();
    commands = new Commands();
    communication = new Comms();

    modules = *new QList<QSharedPointer<AbstractModule>>();

    connect(communication,SIGNAL(devicesScaned(QList<DeviceDescriptor>)),this,SLOT(newDeviceList(QList<DeviceDescriptor>)),Qt::QueuedConnection);
}


void Device::ScanDevices(){
  communication->scanForDevices();
}

void Device::newDeviceList(QList<DeviceDescriptor> deviceList){
    this->deviceList = deviceList;
    qDebug() << "scanned device lis received in device.cpp";
    emit updateDeviceList(deviceList);
}


void Device::open(int deviceIndex){
    communication->open(deviceList.at(deviceIndex));

    while (communication->getIsOpen()==false) {
        QThread::msleep(500);
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();

    connect(communication,&Comms::newData,this,&Device::parseData);
    connect(communication,&Comms::communicationError,this,&Device::handleError);

    communication->write(commands->SYSTEM+":"+commands->CONFIG_REQUEST+";");
    foreach(QSharedPointer<AbstractModule> mod, modules){
       mod->setComms(communication);
    }
}

void Device::close(){
    if(isConnected){
        communication->close();
        foreach(QSharedPointer<AbstractModule> mod, modules){
           mod->closeModule();
        }
        isConnected = communication->getIsOpen();
    }

    disconnect(communication,&Comms::newData,this,&Device::parseData);
    disconnect(communication,&Comms::communicationError,this,&Device::handleError);
}

void Device::handleError(QByteArray error){
    emit fatalError(error);
}

void Device::parseData(QByteArray data){
    bool isDataPassed = false;

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    foreach(QSharedPointer<AbstractModule> mod, modules){
        if(dataHeader == mod->moduleCommandPrefix){
            mod->parseData(dataToPass);
            isDataPassed=true;
        }
    }
        //What if data belongs to voltmeter???????????
        //Solution1: each module has to know if it is running or not and handle data correctly.
        //Solution2: opeed modules will be handeled here

    if(dataHeader=="SYST"){
       parseSystemData(dataToPass);
        isDataPassed=true;
    }

    if(!isDataPassed){
       qDebug() << "ERROR: this data was not passed to any module" << data;
    }
}

void Device::parseSystemData(QByteArray data){
 //   qDebug() << "data are in system parser device.cpp" << data;
    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec->parseSpecification(data);
        emit updateSpecfication();
    }else if(feature=="ACK_"){
      //  qDebug() << "ACK";
    }else{
        qDebug() << "ERROR: unparsable data for system" << feature << " "<< data;
    }
}

bool Device::getIsConnected() const
{
    return isConnected;
}

bool Device::getIsSpecificationLoaded(){
    return deviceSpec->isSpecLoaded;
}

DeviceSpec* Device::getDeviceSpecification(){
    return deviceSpec;
}

QWidget* Device::createModule(ModuleDockWidget *dockWidget ,WidgetControlModule *controlModule, QByteArray cmdPrefix){
    QSharedPointer<AbstractModule>  module (new Scope());

    modules.append(module);

    module->setDockWidgetWindow(dockWidget);
    module->setModuleControlWidget(controlModule);
    module->setCommandPrefix(cmdPrefix);
    return module->getWidget();
}





