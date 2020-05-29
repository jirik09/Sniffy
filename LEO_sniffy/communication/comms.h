#ifndef COMMS_H
#define COMMS_H

#include <QObject>
#include "connectiontype.h"
#include "serialline.h"

class Comms : public QThread
{
    Q_OBJECT
public:
    explicit Comms(QObject *parent = nullptr);
    void run();
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
    void write(QByteArray data);
    void writeUint32(int data);
    void writeUint16(int data);
    void writeString(QString data);

private:

    SerialLine *serial;
    bool isOpen = false;
    bool requestToScan = false;

};

#endif // COMMS_H
