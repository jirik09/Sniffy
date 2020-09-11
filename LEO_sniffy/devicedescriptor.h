#ifndef DEVICEDESCRIPTOR_H
#define DEVICEDESCRIPTOR_H

#include <QMetaType>
#include <QStringList>

#include "communication/connectiontype.h"

class DeviceDescriptor
{
public:
    DeviceDescriptor() = default;
    ~DeviceDescriptor() = default;
    DeviceDescriptor(const DeviceDescriptor &) = default;
    DeviceDescriptor &operator=(const DeviceDescriptor &) = default;

    DeviceDescriptor(const QString &deviceName, const QString &port, const int &speed, const Connection &connType, const int &index);

    QString deviceName;
    QString port;
    int speed;
    Connection connType;
    int index;
};

Q_DECLARE_METATYPE(DeviceDescriptor);
QDebug operator<<(QDebug dbg, const DeviceDescriptor &deviceDesc);

#endif // DEVICEDESCRIPTOR_H
