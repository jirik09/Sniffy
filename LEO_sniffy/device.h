#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>
#include "communication/comms.h"
#include "modules/scope/scope.h"
#include "communication/commands.h"
#include "devicespec.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);
    void open(int deviceIndex);
    void close();
    DeviceSpec* getDeviceSpecification();

    bool getIsConnected() const;
    bool getIsSpecificationLoaded();
    void ScanDevices();

    QWidget* createModule(ModuleDockWidget *dockWidget ,WidgetControlModule *controlModule, QByteArray cmdPrefix);

signals:
  //  void scopeNewData(QByteArray);
    void systemNewData(QByteArray);
    void writeData (QByteArray data);

    void updateDeviceList(QList<DeviceDescriptor> deviceList);
    void updateSpecfication();
    void fatalError(QByteArray);

private slots:
    void parseData(QByteArray data);
    void parseSystemData(QByteArray data);
    void newDeviceList(QList<DeviceDescriptor> deviceList);
    void handleError(QByteArray error);


private:
    QList<DeviceDescriptor> deviceList;

    Comms *communication;
    QList<QSharedPointer<AbstractModule>> modules;

    bool isConnected = false;

    Commands *commands;
    DeviceSpec *deviceSpec;

};

#endif // DEVICE_H
