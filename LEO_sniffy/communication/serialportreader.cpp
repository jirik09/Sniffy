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

#ifdef DEBUG_READER
    if(currentlyRead.length()>48){
        qDebug() <<"READ:" <<currentlyRead.length()<<":"<< currentlyRead.left(16)<<"..."<<currentlyRead.right(16);
    }else{
        qDebug() <<"READ:" <<currentlyRead.length()<<":"<< currentlyRead;
    }
#endif

    emit newData(currentlyRead);
}


