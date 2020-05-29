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

#endif // CONNECTIONTYPE_H
