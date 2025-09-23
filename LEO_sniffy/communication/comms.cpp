#include "comms.h"

Comms::Comms(QObject *parent) : QObject(parent)
{
    //qDebug()<< "thread comms"<<this->thread();
    serial = new SerialLine();
    serialThread = new QThread(this);

    serial->moveToThread(serialThread);
    connect(this,&Comms::dataWrite,serial,&SerialLine::write);
    connect(serial,&SerialLine::newMessage,this,&Comms::parseMessage);
    connect(serial,&SerialLine::serialLineError,this,&Comms::errorReceived);
    connect(this,&Comms::openLine,serial,&SerialLine::openSerialLine);
    connect(this,&Comms::closeLine,serial,&SerialLine::closeLine);
    serialThread->start();

    connect(&devScanner,&DeviceScanner::newDevicesScanned,this,&Comms::devicesScanned);
    devScanner.start();
    devScanner.searchForDevices(true);

}

Comms::~Comms()
{
    // stop background scanner thread
    devScanner.quit();
    devScanner.wait();

    // stop serial worker thread and cleanup worker
    serialThread->quit();
    serialThread->wait();

    if (serial) {
        delete serial;
        serial = nullptr;
    }
}

void Comms::open(DeviceDescriptor device){
        switch (device.connType) {
        case Connection::SERIAL:
            devScanner.searchForDevices(false);
            emit openLine(device);
            break;
        default:
            break;
    }
}

void Comms::scanForDevices(){
    QList<DeviceDescriptor> listDevices;
    SerialLine::getAvailableDevices(&listDevices,0);   //scan for available devices on serial port
    emit devicesScaned(listDevices);
}

void Comms::close(){
    emit closeLine();
    //devScanner.searchForDevices(true);
}

void Comms::write(QByteArray module, QByteArray feature, QByteArray param){

#ifdef DEBUG_COMMS
    QDateTime date = QDateTime::currentDateTime();
    qDebug() <<"COMM_WRITE ("<< date.time()<<"):"<< module+":"+feature+":"+param+";";
#endif
    emit dataWrite(module+":"+feature+":"+param+";");
}

void Comms::write(QByteArray module, QByteArray command){

#ifdef DEBUG_COMMS
    QDateTime date = QDateTime::currentDateTime();
    qDebug() <<"COMM_WRITE ("<< date.time()<<"):"<< module+":"+command;
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
    QDateTime date = QDateTime::currentDateTime();
    qDebug() << "COMM_WRITE ("<< date.time()<<"):"<<module+":"+feature+":" << param <<";";
#endif
    QByteArray qb(tmp,4);
    emit dataWrite(module+":"+feature+" "+qb.left(4)+";");
}

void Comms::write(QByteArray data){

#ifdef DEBUG_COMMS
    QDateTime date = QDateTime::currentDateTime();
    qDebug() << "COMM_WRITE ("<< date.time()<<"):"<<data;
#endif
    emit dataWrite(data);
}

void Comms::errorReceived(QByteArray error){
    emit communicationError(error);
    devScanner.searchForDevices(true);
}


void Comms::devicesScanned(QList<DeviceDescriptor> deviceList)
{
    emit devicesScaned(deviceList);
}

void Comms::parseMessage(QByteArray message){
    //CRC can be checked here but it is not implemented
    //just pass the data
#ifdef DEBUG_COMMS
    QDateTime date = QDateTime::currentDateTime();
    if(message.length()>64){
        qDebug() <<"COMM_READ ("<< date.time()<<"):"<< message.left(40)<<"..."<<message.right(16);
    }else{
        qDebug() <<"COMM_READ ("<< date.time()<<"):"<< message;
    }
#endif
    emit newData(message);
}

bool Comms::getIsOpen() const
{
    return serial->getIsOpen();
}

