#ifndef CONNECTIONTYPE_H
#define CONNECTIONTYPE_H

#include <QObject>

enum class Connection
{
  SERIAL,
  BLUETOOTH,
  ETHERNET,
  USB,
};

typedef struct
{
    QString deviceName;
    QString port;
    int speed;
    Connection connType;
    int index;

} device_descriptor;

#endif // CONNECTIONTYPE_H
