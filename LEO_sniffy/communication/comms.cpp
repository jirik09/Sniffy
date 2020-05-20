#include "comms.h"

Comms::Comms(QObject *parent) : QObject(parent)
{

}

QList<device_descriptor> * Comms::scanForDevices(){

    listDevices = new QList<device_descriptor>;

    SerialLine::getAvailableDevices(listDevices,0);

    return  listDevices;
}

bool Comms::open(int deviceIndex){
    bool ret = false;

    switch (listDevices->at(deviceIndex).connType) {
    case Connection::SERIAL:
         serial = new SerialLine();
         ret = serial->openLine(listDevices->at(deviceIndex));
         connect(serial,SIGNAL(newMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
        break;
    default:
        break;
    }

    return ret;
}

void Comms::write(const char *data){
    serial->write(data);
}

void Comms::write(QByteArray data){
    serial->write(data);
}

void Comms::write(QString data){
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
    emit newData(message);
}

