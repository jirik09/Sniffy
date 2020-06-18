

#ifndef SERIALPORTWRITER_H
#define SERIALPORTWRITER_H

#include <QByteArray>
#include <QObject>
#include <QSerialPort>
#include <QDebug>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class SerialPortWriter : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortWriter(QSerialPort *serialPort, QObject *parent = nullptr);
    void write(const QByteArray &writeData);

private slots:
    void handleBytesWritten(qint64 bytes);

private:
    QSerialPort *m_serialPort = nullptr;
    QByteArray m_writeData;
};

#endif // SERIALPORTWRITER_H
