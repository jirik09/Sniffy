#include "serialportreader.h"

SerialPortReader::SerialPortReader(QSerialPort *serialPort, QObject *parent) :
    QObject(parent),
    m_serialPort(serialPort)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPortReader::handleReadyRead);

}

void SerialPortReader::handleReadyRead()
{
    QByteArray currentlyRead = m_serialPort->readAll();
    sum += currentlyRead.length();

   /* if(currentlyRead.length()>96){
        qDebug() << sum<<"SER:" <<currentlyRead.length()<<":"<< currentlyRead.left(32)<<"..."<<currentlyRead.right(16);
    }else{
        qDebug() << sum <<"SER:" <<currentlyRead.length()<<":"<< currentlyRead;
    }*/

    emit newData(currentlyRead);
}


