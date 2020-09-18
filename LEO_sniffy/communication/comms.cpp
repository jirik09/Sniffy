#include "comms.h"

Comms::Comms(QObject *parent) : QObject(parent)
{
}

void Comms::open(DeviceDescriptor device){
        switch (device.connType) {
        case Connection::SERIAL:
            isOpen = serial->openLine(device);
            connect(serial,SIGNAL(newMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
            connect(serial,SIGNAL(serialLineError(QByteArray)),this,SLOT(errorReceived(QByteArray)));
            break;
        default:
            break;
    }
}

void Comms::scanForDevices(){
    QList<DeviceDescriptor> listDevices = *new QList<DeviceDescriptor>;
    serial = new SerialLine();
    serial->getAvailableDevices(&listDevices,0);   //scan for available devices on serial port
    emit devicesScaned(listDevices);
}

void Comms::close(){
    isOpen = false;
    disconnect(serial,SIGNAL(newMessage(QByteArray)),this,SLOT(parseMessage(QByteArray)));
    disconnect(serial,SIGNAL(serialLineError(QByteArray)),this,SLOT(errorReceived(QByteArray)));
    serial->closeLine();
}

void Comms::write(QByteArray module, QByteArray feature, QByteArray param){
    serial->write(module+":"+feature+":"+param+";");
#ifdef DEBUG_COMMS
    qDebug() << module+":"+feature+":"+param+";";
#endif
}

void Comms::write(QByteArray module, QByteArray command){
    serial->write(module+":"+command+";");
#ifdef DEBUG_COMMS
    qDebug() << module+":"+command;
#endif
}

void Comms::write(QByteArray module, QByteArray feature, int param){
    char tmp[4] = {0};
    tmp[3] = (param>>24);
    tmp[2] = (param>>16);
    tmp[1] = (param>>8);
    tmp[0] = (param);
    serial->write(module+":"+feature+" ");
    serial->write(tmp,4);
    serial->write(";");
#ifdef DEBUG_COMMS
    qDebug() << module+":"+feature+":" << param <<";";
#endif
}

void Comms::write(QByteArray data){
    serial->write(data);
#ifdef DEBUG_COMMS
    qDebug() << data;
#endif

}

void Comms::errorReceived(QByteArray error){
    emit communicationError(error);
}

void Comms::parseMessage(QByteArray message){
    //CRC can be checked here but it is not implemented
    //just pass the data
#ifdef DEBUG_COMMS
    if(message.length()>96){
        qDebug() << message.left(64)<<"..."<<message.right(16);
    }else{
        qDebug() << message;
    }
#endif
    emit newData(message);
}

bool Comms::getIsOpen() const
{
    return isOpen;
}

