#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{
    deviceSpec = new DeviceSpec();
    commands = new Commands();
    scope = new Scope();
}

void Device::startCommunication(){
    communication = new Comms(this);
    connect(communication,SIGNAL(devicesScaned(QList<DeviceDescriptor>)),this,SLOT(newDeviceList(QList<DeviceDescriptor>)),Qt::QueuedConnection);

    communication->start();
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

    connect(this,SIGNAL(writeData(QByteArray)),communication,SLOT(write(QByteArray)),Qt::QueuedConnection);
    connect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));
    connect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));
    connect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));

    while (communication->getIsOpen()==false) {
        QThread::msleep(500);
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();
    scope->setComms(communication);

}

void Device::close(){
    disconnect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));
    disconnect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));
    disconnect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));
    if(isConnected){
        communication->close();
        isConnected = communication->getIsOpen();

    }
}



void Device::loadHWSpecification(void){

    communication->write(commands->SYSTEM+":"+commands->CONFIG_REQUEST+";");
    communication->write(commands->SCOPE+":"+commands->CONFIG_REQUEST+";");

}

void Device::parseData(QByteArray data){

    //qDebug() << "device Parse:" << data;

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    if(dataHeader=="OSCP"){
       emit scopeNewData(dataToPass);
        //What if data belongs to scope??????????? kurva device bud musi vedet co je open nebo module kdyz dostane data tak musi vedet ze neni aktivni.
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

Scope* Device::getScope(){
    return scope;
}




