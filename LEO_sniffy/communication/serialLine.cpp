#include "serialline.h"

SerialLine::SerialLine(QObject *parent) : QObject(parent)
{

}

int SerialLine::getAvailableDevices(QList<DeviceDescriptor> *list, int setFirstIndex){


    QSerialPortInfo *portInfo = new QSerialPortInfo();
    QSerialPort *sPort;

    QList<QSerialPortInfo> ports;

  //  connect(serPort, SIGNAL(readyRead()), this, SLOT(receiveData()));
  //  connect(serPort, &QSerialPort::errorOccurred, this, &SerialLine::handleError);

    ports = portInfo->availablePorts();

    QSerialPortInfo tmpPort;
    int numberOfDevices = 0;
    QByteArray received;
    DeviceDescriptor desc;

    foreach (tmpPort, ports){

        QSerialPortInfo portIn (tmpPort.portName());

        sPort = new QSerialPort(portIn,nullptr);

        sPort->setBaudRate(921600);
        sPort->setDataBits(QSerialPort::DataBits::Data8);
        sPort->setParity(QSerialPort::Parity::NoParity);
        sPort->setStopBits(QSerialPort::StopBits::OneStop);

        if(sPort->open(QIODevice::ReadWrite)){
            sPort->clear();
            sPort->write("IDN?;");
            sPort->waitForBytesWritten();
            sPort->waitForReadyRead(200);

            received = sPort->readAll();

           // qDebug() <<sPort->portName()<<received.length()<<received;
            if (received.length()>16 && received.left(4)=="SYST"){
                sPort->write("RES!;");
                sPort->waitForBytesWritten();

              /*  thread()->msleep(150);
                sPort->write("IDN?;");
                sPort->waitForBytesWritten();

                //thread()->msleep(100);
                sPort->waitForReadyRead(150);

                received = sPort->readAll();*/
                desc.port = sPort->portName();
                desc.speed = sPort->baudRate();
                desc.connType = Connection::SERIAL;
                desc.index = setFirstIndex + numberOfDevices;
                desc.deviceName = received.right(received.length()-4).left(received.length()-8);
                list->append(desc);
                numberOfDevices++;
            }
            sPort->close();
        }
    }
    return numberOfDevices;
}

void SerialLine::openLine(DeviceDescriptor desc){

    //qDebug()<< "serial line open"<<this->thread();
    bool success = false;
    serPort = new QSerialPort();
    serPort->setPortName(desc.port);
    serPort->setBaudRate(desc.speed);

    buffer = new QByteArray();
    message = new QByteArray();

    if(serPort->open(QIODevice::ReadWrite)){
        success=true;
        serialReader = new SerialPortReader(serPort,this);
        serialWriter = new SerialPortWriter(serPort,this);

        connect(serialReader,&SerialPortReader::newData, this, &SerialLine::receiveData);
        connect(serPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    }else{
        qDebug () << "Serial port error opening";
    }

    isOpen=success;
}

void SerialLine::closeLine(){
    serPort->flush();
    serPort->close();
    //serPort->clear();
    isOpen = false;
}

void SerialLine::handleError(QSerialPort::SerialPortError error){
    qDebug()<<"FATAL ERROR occured on serial line" << error;

    QByteArray errorString;
    QDataStream out(&errorString, QIODevice::WriteOnly);

    emit serialLineError("SerialLine Error");
}


void SerialLine::receiveData(QByteArray data){
    buffer->append(data);

    //find delimiter
    int i = buffer->indexOf(delimiter);
    while(i>0){
        message = new QByteArray(buffer->left(i));
      //  qDebug() << "Received:" << *message;
        emit newMessage(*message);
        //remove message and delimiter from buffer
        buffer->remove(0,i+4);

        i = buffer->indexOf(delimiter);
    }
}

bool SerialLine::getIsOpen() const
{
    return isOpen;
}

void SerialLine::write(const QByteArray &writeData){
    QDateTime date = QDateTime::currentDateTime();
    //qDebug() << "Sent" <<  date.time() <<":" << writeData;
    if(serPort->isOpen()){
        serialWriter->write(writeData);
    }else{
        qDebug() << "ERROR cannot send data: port is closed";
    }
}

/*void SerialLine::write(const char *data, qint32 len){
    QDateTime date = QDateTime::currentDateTime();
   // qDebug() << "Sent" <<  date.time() <<":" << data;
    if(serPort->isOpen()){
        serialWriter->write(data,len);
    }else{
        qDebug() << "ERROR cannot send data: port is closed";
    }
}*/


