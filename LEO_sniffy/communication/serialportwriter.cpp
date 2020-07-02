#include "serialportwriter.h"

SerialPortWriter::SerialPortWriter(QSerialPort *serialPort, QObject *parent) :
    QObject(parent),
    m_serialPort(serialPort)
{
    connect(m_serialPort, &QSerialPort::bytesWritten,this, &SerialPortWriter::handleBytesWritten);
}

void SerialPortWriter::handleBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
  //  qDebug() << "Bytes Written" << bytes;
}


void SerialPortWriter::write(const QByteArray &writeData)
{
    m_writeData = writeData;
    const qint64 bytesWritten = m_serialPort->write(writeData);
    if (bytesWritten == -1) {
        qDebug() << "ERROR no data was send";
    } else if (bytesWritten != m_writeData.size()) {
        qDebug() << "ERROR data was send only partially" << bytesWritten << "/" << m_writeData.size();
    }
}

void SerialPortWriter::write(const char *writeData, qint32 len)
{
    m_writeData = writeData;
    const qint64 bytesWritten = m_serialPort->write(writeData,len);
    if (bytesWritten == -1) {
        qDebug() << "ERROR no data was send";
    } else if (bytesWritten < len) {
        qDebug() << "ERROR data was send only partially" << bytesWritten << "/" << m_writeData.size();
    }
}
