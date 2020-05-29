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
            serial->getAvailableDevices(&listDevices,0);   //scan fo available devices on serial port
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
    serial->closeLine();
}

void Comms::write(QByteArray data){
    serial->write(data);
}

void Comms::writeString(QString data){
    QByteArray tmp = data.toUtf8();
    serial->write(tmp);
}

void Comms::writeUint32(int data){
    QByteArray tmp (4,0);
    tmp[3] = data>>24;
    tmp[2] = data>>16;
    tmp[1] = data>>8;
    tmp[0] = data;
    serial->write(tmp);
}

void Comms::writeUint16(int data){
    QByteArray tmp (2,0);
    tmp[1] = data>>8;
    tmp[0] = data;
    serial->write(tmp);
}


void Comms::parseMessage(QByteArray message){
    //CRC can be checked here but it is not implemented
    //just pass the data
    qDebug() << "parse massage in coms.cpp";
    emit newData(message);
}

bool Comms::getIsOpen() const
{
    return isOpen;
}

