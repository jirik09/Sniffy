#include "devicedescriptor.h"

#include <QDebug>

DeviceDescriptor::DeviceDescriptor(const QString &deviceName, const QString &port, const int &speed, const Connection &connType, const int &index)
    :deviceName(deviceName),port(port),speed(speed),connType(connType),index(index)
{
}

QDebug operator<<(QDebug dbg, const DeviceDescriptor &deviceDesc)
{
    dbg.nospace() << "Name:" << deviceDesc.deviceName << " Port:" << deviceDesc.port << " Speed:" << deviceDesc.speed;
    return dbg.maybeSpace();
}


