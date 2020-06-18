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
  //  qDebug() << currentlyRead;

    emit newData(currentlyRead);
}


