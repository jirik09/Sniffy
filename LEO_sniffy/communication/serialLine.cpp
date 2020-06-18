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

            sPort->write("IDN?;");
            sPort->waitForBytesWritten();

            QThread::msleep(300);
            sPort->waitForReadyRead(200);

            received = sPort->readAll();
   //         qDebug(" %d bytes received",received.length());
            if (received.length()>1){
               /* sPort->write("RES;");
                sPort->waitForBytesWritten();

                thread()->msleep(200);
                sPort->write("IDN?;");
                sPort->waitForBytesWritten();

                thread()->msleep(200);
                sPort->waitForReadyRead(50);*/
                received = sPort->readAll();

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

bool SerialLine::openLine(DeviceDescriptor desc){

    bool ret=false;
    serPort = new QSerialPort();
  //  connect(serPort, &QSerialPort::readyRead, this, &SerialLine::receiveData);
    serPort->setPortName(desc.port);
    serPort->setBaudRate(desc.speed);
    serPort->setDataBits(QSerialPort::DataBits::Data8);
    serPort->setParity(QSerialPort::Parity::NoParity);
    serPort->setStopBits(QSerialPort::StopBits::OneStop);

    if(serPort->open(QIODevice::ReadWrite)){
        ret=true;
        buffer = new QByteArray();
        message = new QByteArray();
       // connect(serPort, SIGNAL(errorOccurred()), this, SLOT(handleError()));
       // connect(serPort, SIGNAL(readyRead()), this, SLOT(receiveData()),Qt::QueuedConnection);
       // connect(serPort, &QSerialPort::errorOccurred, this, &SerialLine::handleError);

    }else{
        qDebug () << "Serial port error opening";
    }
    return ret;
}

void SerialLine::closeLine(){
    serPort->close();
}

void SerialLine::handleError(QSerialPort::SerialPortError error){
    qDebug()<<"Error occured on serial line" << error;

}

void SerialLine::receiveData(){

    serPort->waitForReadyRead(10);
    //

    //append data to buffer
    if(serPort->bytesAvailable()>0) {
     //   qDebug() << "Incomming" << serPort->bytesAvailable();
        buffer->append(serPort->readAll());


        //qDebug() << buffer->length() << ":" << *buffer;

        //find delimiter
        int i = buffer->indexOf(delimiter);
        while(i>0){

            message = new QByteArray(buffer->left(i));

          //  qDebug() << "Received:" << *message;
            emit newMessage(*message);

            //remove message and delimiter from buffer
            buffer->remove(0,i+4);

            //example how to read the int from serial line
            /*  QDataStream ds(message);
            int j;
            ds>>j;*/


            //try to find new delimiter and message
            i = buffer->indexOf(delimiter);
        } }
}

void SerialLine::write(const char *data){
    //qDebug() << "Sent:" << data;
    if(serPort->isOpen()){
        serPort->write(data);
        serPort->waitForBytesWritten();
    }else{

        qDebug() << "ERROR cannot send data: port is closed";
    }
}

