#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{
    deviceSpec = new DeviceSpec();
    commands = new Commands();
    scope = new Scope();
    communication = new Comms();
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

    connect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));
    connect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));
    connect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));
    connect(communication,&Comms::communicationError,this,&Device::handleError);

    communication->write(commands->SYSTEM+":"+commands->CONFIG_REQUEST+";");

    scope->setComms(communication,commands->SCOPE);
}

void Device::close(){
    if(isConnected){
        communication->close();
        scope->closeModule();
        isConnected = communication->getIsOpen();
    }

    //All the signals conected above has to be disconnected here!!!
    //multiple connections cause multiple calls and multiple data receive
    disconnect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));

    disconnect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));
    disconnect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));

    disconnect(communication,&Comms::communicationError,this,&Device::handleError);
}

void Device::handleError(QByteArray error){
    emit fatalError(error);
}

void Device::parseData(QByteArray data){

    //qDebug() << "device Parse:" << data;

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    if(dataHeader=="OSCP"){
       emit scopeNewData(dataToPass);
        //What if data belongs to voltmeter???????????
        //Solution1: each module has to know if it is running or not and handle data correctly.
        //Solution2: opeed modules will be handeled here
    }else if(dataHeader=="SYST"){
       emit systemNewData(dataToPass);
    }else{
       qDebug() << "ERROR: this data could not be passed into module" << data;
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

void Device::createModule(WindowScope *moduleWidget,ModuleDockWidget *dockWidget ,WidgetControlModule *controlModule){
    scope->setModuleWindow(moduleWidget);
    scope->setDockWidgetWindow(dockWidget);
    scope->setModuleControlWidget(controlModule);
}

Scope* Device::getScope(){
    return scope;
}




