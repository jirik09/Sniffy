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

class ConnectionType : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionType(QObject *parent = nullptr);
    virtual int getAvailableDevices(QList<device_descriptor> *list, int setFirstIndex) = 0;
   // virtual int send(QString text) = 0;
  //  virtual int send(int number) = 0;

public: signals:
   //virtual void recieve(int length) = 0;

};

#endif // CONNECTIONTYPE_H
