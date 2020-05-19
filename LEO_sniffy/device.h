#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include "communication/comms.h"
#include "features/scope/scope.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr, Comms *comm = nullptr, int deviceToOpen = -1);
    bool connect(int PortIndex);
    void loadSpecification(void);
    void parseMessage(void);

signals:

private:
    Comms *communication;
    Scope *scope;
    bool connected = false;

};

#endif // DEVICE_H
