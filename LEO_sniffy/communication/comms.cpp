#include "comms.h"

Comms::Comms(QObject *parent) : QObject(parent)
{
    //qDebug()<< "thread comms"<<this->thread();
    serial = new SerialLine();
    worker = new QThread(this);

    serial->moveToThread(worker);
    connect(this,&Comms::dataWrite,serial,&SerialLine::write);
    connect(serial,&SerialLine::newMessage,this,&Comms::parseMessage);
    connect(serial,&SerialLine::serialLineError,this,&Comms::errorReceived);
    connect(this,&Comms::openLine,serial,&SerialLine::openLine);
    connect(this,&Comms::closeLine,serial,&SerialLine::closeLine);
    worker->start();

}

void Comms::open(DeviceDescriptor device){
        switch (device.connType) {
        case Connection::SERIAL:
            emit openLine(device);
            break;
        default:
            break;
    }
}

void Comms::scanForDevices(){
    QList<DeviceDescriptor> listDevices = *new QList<DeviceDescriptor>;
    SerialLine::getAvailableDevices(&listDevices,0);   //scan for available devices on serial port
    emit devicesScaned(listDevices);
}

void Comms::close(){
    isOpen = false;
    emit closeLine();
}

void Comms::write(QByteArray module, QByteArray feature, QByteArray param){

#ifdef DEBUG_COMMS
    qDebug() <<"COMM_WRITE:"<< module+":"+feature+":"+param+";";
#endif
    emit dataWrite(module+":"+feature+":"+param+";");
}

void Comms::write(QByteArray module, QByteArray command){

#ifdef DEBUG_COMMS
    qDebug() <<"COMM_WRITE:"<< module+":"+command;
#endif
    emit dataWrite(module+":"+command+";");
}

void Comms::write(QByteArray module, QByteArray feature, int param){
    char tmp[4] = {0};
    tmp[3] = (param>>24);
    tmp[2] = (param>>16);
    tmp[1] = (param>>8);
    tmp[0] = (param);

#ifdef DEBUG_COMMS
    qDebug() << "COMM_WRITE:"<<module+":"+feature+":" << param <<";";
#endif

    QByteArray *qb = new QByteArray(tmp,4);
    emit dataWrite(module+":"+feature+" "+qb->left(4)+";");
}

void Comms::write(QByteArray data){

#ifdef DEBUG_COMMS
    qDebug() << "COMM_WRITE:"<<data;
#endif
    emit dataWrite(data);
}

void Comms::errorReceived(QByteArray error){
    emit communicationError(error);
}

void Comms::parseMessage(QByteArray message){
    //CRC can be checked here but it is not implemented
    //just pass the data
#ifdef DEBUG_COMMS
    if(message.length()>96){
        qDebug() <<"COMM_READ:"<< message.left(64)<<"..."<<message.right(16);
    }else{
        qDebug() <<"COMM_READ:"<< message;
    }
#endif
    emit newData(message);
}

bool Comms::getIsOpen() const
{
    return serial->getIsOpen();
}

