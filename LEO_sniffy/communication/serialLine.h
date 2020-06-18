#ifndef SERIALLINE_H
#define SERIALLINE_H

#include <QObject>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include <QBuffer>
#include <QDateTime>

#include "connectiontype.h"
#include "devicedescriptor.h"

#include "serialportreader.h"
#include "serialportwriter.h"

#include <QDebug>


class SerialLine : public QObject
{
    Q_OBJECT
public:
    explicit SerialLine(QObject *parent = nullptr);

    int getAvailableDevices(QList<DeviceDescriptor> *list, int setFirstIndex);

    bool openLine(DeviceDescriptor desc);
    void closeLine();
    void write(const char *data);


signals:
        void newMessage(QByteArray message);
        void serialLineError();

private slots:
        void handleError(QSerialPort::SerialPortError error);
        void receiveData(QByteArray data);

private:
    QSerialPort *serPort;
    QByteArray *buffer;

    QByteArray *message;

    const char delimiterRaw[4] = { '\xCA', '\xFE', '\xFA', '\xDE'};
    const QByteArray delimiter = QByteArray::fromRawData(delimiterRaw,4);

    SerialPortReader *serialReader;
    SerialPortWriter *serialWriter;
};

#endif // SERIALLINE_H
