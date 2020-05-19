#ifndef COMMS_H
#define COMMS_H

#include <QObject>
#include "connectiontype.h"
#include "serialline.h"

class Comms : public QObject
{
    Q_OBJECT
public:
    explicit Comms(QObject *parent = nullptr);
    QList<device_descriptor> * scanForDevices();
    bool open(int deviceIndex);


signals:

private:
    QList<device_descriptor> *listDevices;
    SerialLine *serial;

};

#endif // COMMS_H
