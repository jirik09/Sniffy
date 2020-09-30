#ifndef COMMS_H
#define COMMS_H

#include <QObject>
#include <QMutex>
#include "connectiontype.h"
#include "serialline.h"



class Comms : public QObject
{
    Q_OBJECT

//#define DEBUG_COMMS

public:
    explicit Comms(QObject *parent = nullptr);
    void close();
    void open(DeviceDescriptor device);
    bool getIsOpen() const;
    void scanForDevices();

    void write(QByteArray module, QByteArray feature, QByteArray param);
    void write(QByteArray module, QByteArray feature, int param);
    void write(QByteArray module, QByteArray command);
    void write(QByteArray data);

signals:
    void dataWrite(const QByteArray &data);
    void openLine(DeviceDescriptor desc);
    void closeLine();
    void newData(QByteArray message);
    void devicesScaned(QList<DeviceDescriptor> deviceList);
    void communicationError(QByteArray);

private slots:
    void parseMessage(QByteArray message);
    void errorReceived(QByteArray error);

private:
    SerialLine *serial;
    bool isOpen = false;
    QThread *worker;
};

#endif // COMMS_H
