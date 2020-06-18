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

public:
    explicit SerialPortReader(QSerialPort *serialPort, QObject *parent = nullptr);

signals:
    void newData(QByteArray);

private slots:
    void handleReadyRead();

private:
    QSerialPort *m_serialPort = nullptr;
    QByteArray m_readData;

};

#endif // SERIALPORTREADER_H
