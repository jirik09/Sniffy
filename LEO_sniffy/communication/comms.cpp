#include "comms.h"

Comms::Comms(QObject *parent)
{

}


void Comms::run(){
//    qDebug() << "devices scanned in a thread";

    forever{
        if(isOpen){
            serial->receiveData();
        }else{
            thread()->msleep(100);
        }
        if(requestToScan){
            QList<DeviceDescriptor> listDevices = *new QList<DeviceDescriptor>;
            serial = new SerialLine();
            serial->getAvailableDevices(&listDevices,0);   //scan for available devices on serial port
            emit devicesScaned(listDevices);
            requestToScan = false;
        }
    }
    exec();
}

void Comms::open(DeviceDescriptor device){
        switch (device.connType) {
        case Connection::SERIAL:
            isOpen = serial->openLine(device);
            connect(serial,SIGNAL(newMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
            break;
        default:
            break;
    }
}

void Comms::scanForDevices(){
    requestToScan = true;
}

void Comms::close(){
    isOpen = false;
    disconnect(serial,SIGNAL(newMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
    serial->closeLine();
}

void Comms::write(QByteArray module, QByteArray feature, QByteArray param){
    serial->write(module+":"+feature+":"+param+";");
}

void Comms::write(QByteArray module, QByteArray feature, int param){
    QByteArray tmp (4,0);
    tmp[3] = param>>24;
    tmp[2] = param>>16;
    tmp[1] = param>>8;
    tmp[0] = param;
    serial->write(module+":"+feature+" "+tmp+";");
}

void Comms::write(QByteArray data){
    serial->write(data);
}



void Comms::parseMessage(QByteArray message){
    //CRC can be checked here but it is not implemented
    //just pass the data
    //qDebug() << "parse massage in comms.cpp";
    emit newData(message);
}

bool Comms::getIsOpen() const
{
    return isOpen;
}

