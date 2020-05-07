#ifndef COMMS_H
#define COMMS_H

#include <QObject>
#include "connectiontype.h"
#include "serial.h"

class Comms : public QObject
{
    Q_OBJECT
public:
    explicit Comms(QObject *parent = nullptr);
    QList<device_descriptor>* scanForDevices();


signals:

private:
    QList<device_descriptor> *listDevices;
    Serial *ser;

};

#endif // COMMS_H
