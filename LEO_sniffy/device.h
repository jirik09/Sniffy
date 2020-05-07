#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "comms.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);
    QList<device_descriptor> scan ();

signals:

private:
    Comms *comunication;

};

#endif // DEVICE_H
