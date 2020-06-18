#ifndef COMMS_H
#define COMMS_H

#include <QObject>
#include <QMutex>
#include "connectiontype.h"
#include "serialline.h"

class Comms : public QObject
{
    Q_OBJECT
public:
    explicit Comms(QObject *parent = nullptr);
    void close();
    void open(DeviceDescriptor device);
    bool getIsOpen() const;
    void scanForDevices();

signals:
    void newData(QByteArray message);
    void devicesScaned(QList<DeviceDescriptor> deviceList);

private slots:
    void parseMessage(QByteArray message);

public slots:
    void write(QByteArray module, QByteArray feature, QByteArray param);
    void write(QByteArray module, QByteArray feature, int param);
    void write(QByteArray data);

private:
    SerialLine *serial;
    bool isOpen = false;
};

#endif // COMMS_H
