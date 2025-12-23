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
#include <atomic>


class SerialLine : public QObject
{
    Q_OBJECT

#define delimiterRaw  "\xCA\xFE\xFA\xDE"

public:
    explicit SerialLine(QObject *parent = nullptr);
    ~SerialLine();

    // Non-copyable (manages owning pointers / OS handle)
    SerialLine(const SerialLine&) = delete;
    SerialLine& operator=(const SerialLine&) = delete;
    SerialLine(SerialLine&&) = delete;
    SerialLine& operator=(SerialLine&&) = delete;

    static int getAvailableDevices(QList<DeviceDescriptor> *list, int setFirstIndex);
    bool getIsOpen() const;

public slots:
    void write(const QByteArray &writeData);
    void openSerialLine(DeviceDescriptor desc);
    void closeLine();

signals:
        void newMessage(QByteArray message);
        void serialLineError(QByteArray error);
        void connectionOpened(bool success);

private slots:
        void handleError(QSerialPort::SerialPortError error);
        void receiveData(QByteArray data);

private:

    void resetPort();
    QSerialPort *serPort = nullptr;
    QByteArray buffer;   // reused buffer to minimize allocs
    QByteArray message;  // reused message container

    const QByteArray delimiter = QByteArray::fromRawData(delimiterRaw,4);

    SerialPortReader *serialReader = nullptr; // owned via QObject parent
    SerialPortWriter *serialWriter = nullptr; // owned via QObject parent
    bool isOpen = false;
};

#endif // SERIALLINE_H
