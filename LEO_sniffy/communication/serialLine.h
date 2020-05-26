#ifndef SERIALLINE_H
#define SERIALLINE_H

#include <QObject>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include <QBuffer>
#include "connectiontype.h"

#include <QDebug>




class SerialLine : public QObject
{
    Q_OBJECT
public:
    explicit SerialLine(QObject *parent = nullptr);

    static int getAvailableDevices(QList<device_descriptor> *list, int setFirstIndex);

    bool openLine(device_descriptor desc);
    void closeLine();
    void write(const char *data);

signals:
        void newMessage(QByteArray message);

private slots:
        void handleError(QSerialPort::SerialPortError error);
        void receiveData(void);

private:

    //QSerialPortInfo *portInfo;
    QSerialPort *serPort;
    QByteArray *buffer;
    QDataStream *bufferReadStream;

    QByteArray *message;

    const char delimiterRaw[4] = { '\xCA', '\xFE', '\xFA', '\xDE'};
    const QByteArray delimiter = QByteArray::fromRawData(delimiterRaw,4);

};

#endif // SERIALLINE_H
