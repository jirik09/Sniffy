#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{
    deviceSpec = new DeviceSpec();

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
        qDebug() << "wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();
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


void Device::write(QByteArray data){
   emit writeData (data);
//   communication->write(data);
}

void Device::loadHWSpecification(void){

    //write((commands.RESET_DEVICE+";").toUtf8());
    //QThread::msleep(100);

    write((commands.SYSTEM+":"+commands.CONFIG_REQUEST+";").toUtf8());
    write((commands.SCOPE+":"+commands.CONFIG_REQUEST+";").toUtf8());

}

void Device::parseData(QByteArray data){

    qDebug() << "deive Parse:" << data;

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    if(dataHeader=="OSCP"){
       emit scopeNewData(dataToPass);
    }else if(dataHeader=="SYST"){
       emit systemNewData(dataToPass);
    }else{
       qDebug() << "ERROR: this data could not be passed into module" << data;
    }
}

void Device::parseSystemData(QByteArray data){
    qDebug() << "data are in system parser device.cpp" << data;

    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec->parseSpecification(data);
        emit updateSpecfication();
    }else if(feature=="ACK_"){
        //do nothing
    }else{
        qDebug() << "ERROR: unparsable data for system" << data;
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



