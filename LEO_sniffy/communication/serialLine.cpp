#include "serialline.h"

SerialLine::SerialLine(QObject *parent) : QObject(parent)
{

}

int SerialLine::getAvailableDevices(QList<device_descriptor> *list, int setFirstIndex){

    QSerialPortInfo *portInfo = new QSerialPortInfo();
    QSerialPort *sPort;

    QList<QSerialPortInfo> ports;
    ports = portInfo->availablePorts();

    QSerialPortInfo tmpPort;
    int numberOfDevices = 0;
    QByteArray received;
    device_descriptor desc;

    foreach (tmpPort, ports){

        QSerialPortInfo portIn (tmpPort.portName());

       /* qDebug("%s",portIn.description().toStdString().c_str());
        qDebug("%s",portIn.manufacturer().toStdString().c_str());
        qDebug("%s",portIn.serialNumber().toStdString().c_str());
        qDebug("%s",portIn.systemLocation().toStdString().c_str());
        qDebug("%d",portIn.vendorIdentifier());
        qDebug("%d",portIn.productIdentifier());*/


        sPort = new QSerialPort(portIn,nullptr);

        sPort->setBaudRate(921600);
        sPort->setDataBits(QSerialPort::DataBits::Data8);
        sPort->setParity(QSerialPort::Parity::NoParity);
        sPort->setStopBits(QSerialPort::StopBits::OneStop);

        if(sPort->open(QIODevice::ReadWrite)){


            sPort->write("IDN?;");
            //sPort->flush();
            sPort->waitForBytesWritten();

            QThread::msleep(50);
            sPort->waitForReadyRead(50);

            received = sPort->readAll();
   //         qDebug(" %d bytes received",received.length());
            if (received.length()>1){
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

bool SerialLine::openLine(device_descriptor desc){

    bool ret=false;

    serPort = new QSerialPort(desc.port);
    serPort->setBaudRate(desc.speed);
    serPort->setDataBits(QSerialPort::DataBits::Data8);
    serPort->setParity(QSerialPort::Parity::NoParity);
    serPort->setStopBits(QSerialPort::StopBits::OneStop);

    if(serPort->open(QIODevice::ReadWrite)){
        ret=true;
        connect(serPort, SIGNAL(errorOccurred()), this, SLOT(handleError()));
        connect(serPort, SIGNAL(readyRead()), this, SLOT(receiveData()));
    }
    return ret;
}

void SerialLine::handleError(void){


}

void SerialLine::receiveData(void){
    //QMessageBox(this, "Error","Serial line received some error");

}

