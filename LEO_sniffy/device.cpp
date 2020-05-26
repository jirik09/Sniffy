#include "device.h"

Device::Device(QObject *parent, Comms *comm) : QObject(parent)
{
    communication = comm;
    deviceSpec = new DeviceSpec();

    scope = new Scope();
}


void Device::open(int deviceIndex){
    isConnected = communication->open(deviceIndex);
    if(isConnected){
        connect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));
        connect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));
        connect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));
    }
}

void Device::close(){
    if(isConnected){
        communication->close();
        disconnect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));
        disconnect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));
        disconnect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));
    }
}


void Device::write(const char *data){
    communication->write(data);
}

void Device::loadHWSpecification(void){

    communication->write(commands.RESET_DEVICE+";");
    QThread::msleep(100);

    communication->write(commands.SYSTEM+":"+commands.CONFIG_REQUEST+";");
    communication->write(commands.SCOPE+":"+commands.CONFIG_REQUEST+";");

}

void Device::parseData(QByteArray data){

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
    qDebug() << "data are in system parser" << data;

    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec->parseSpecification(data);
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



