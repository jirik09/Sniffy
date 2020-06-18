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

            QThread::msleep(50);
            sPort->waitForReadyRead(50);

            received = sPort->readAll();
   //         qDebug(" %d bytes received",received.length());
            if (received.length()>16){
                sPort->write("RES!;");
                sPort->waitForBytesWritten();

                thread()->msleep(50);
                sPort->write("IDN?;");
                sPort->waitForBytesWritten();

                thread()->msleep(50);
                sPort->waitForReadyRead(50);

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

    bool ret = false;
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
        connect(serPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    }else{
        qDebug () << "Serial port error opening";
    }
    return ret;
}

void SerialLine::closeLine(){
    serPort->flush();
    serPort->close();
    serPort->clear();
}

void SerialLine::handleError(QSerialPort::SerialPortError error){
    qDebug()<<"FATAL ERROR occured on serial line" << error;

}

void SerialLine::receiveData(){

    serPort->waitForReadyRead(10);

    if(serPort->bytesAvailable()>0) {
        buffer->append(serPort->readAll());

        //find delimiter
        int i = buffer->indexOf(delimiter);
        while(i>0){
            message = new QByteArray(buffer->left(i));
          //  qDebug() << "Received:" << *message;
            emit newMessage(*message);
            //remove message and delimiter from buffer
            buffer->remove(0,i+4);

            //try to find new delimiter and message
            i = buffer->indexOf(delimiter);
        }
    }
}

void SerialLine::write(const char *data){
    QDateTime date = QDateTime::currentDateTime();
   // qDebug() << "Sent" <<  date.time() <<":" << data;
    if(serPort->isOpen()){
        serPort->write(data);
        serPort->waitForBytesWritten();
    }else{
        qDebug() << "ERROR cannot send data: port is closed";
    }
}

