#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QByteArray>
#include <QSerialPort>
#include <QDebug>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class SerialPortReader : public QObject
{
    Q_OBJECT
    //#define DEBUG_READER

public:
    explicit SerialPortReader(QSerialPort *serialPort, QObject *parent = nullptr);

signals:
    void newData(QByteArray);

private slots:
    void handleReadyRead();

private:
    QSerialPort *m_serialPort = nullptr;
    QByteArray m_readData;
    qint64 sum =0;

};

#endif // SERIALPORTREADER_H
