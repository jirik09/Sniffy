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

#define delimiterRaw  "\xCA\xFE\xFA\xDE"

public:
    explicit SerialLine(QObject *parent = nullptr);

    static int getAvailableDevices(QList<DeviceDescriptor> *list, int setFirstIndex);
    bool getIsOpen() const;

public slots:
    void write(const QByteArray &writeData);
    void openSerialLine(DeviceDescriptor desc);
    void closeLine();

signals:
        void newMessage(QByteArray message);
        void serialLineError(QByteArray error);

private slots:
        void handleError(QSerialPort::SerialPortError error);
        void receiveData(QByteArray data);

private:
    QSerialPort *serPort;
    QByteArray buffer;
    QByteArray message;

   const QByteArray delimiter = QByteArray::fromRawData(delimiterRaw,4);

    SerialPortReader *serialReader;
    SerialPortWriter *serialWriter;
    bool isOpen = false;
};

#endif // SERIALLINE_H
