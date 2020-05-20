#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QThread>
#include "communication/comms.h"
#include "features/scope/scope.h"
#include "communication/commands.h"
#include "devicespec.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr, Comms *comm = nullptr);
    void open(int deviceIndex);
    void loadHWSpecification(void);
    void write(const char *data);


signals:
    void scopeNewData(QByteArray);
    void systemNewData(QByteArray);

private slots:
    void parseData(QByteArray data);
    void parseSystemData(QByteArray data);

private:

    Comms *communication;
    Scope *scope;
    bool isConnected = false;

    Commands commands;
    DeviceSpec specification;

};

#endif // DEVICE_H
