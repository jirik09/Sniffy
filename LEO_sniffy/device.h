#ifndef DEVICE_H
#define DEVICE_H

//#include <QObject>
//#include <QThread>
//#include <QSharedPointer>
#include "communication/comms.h"
#include "communication/commands.h"
#include "modules/scope/scope.h"
#include "modules/scan/scan.h"
#include "devicespec.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);
    QList<QSharedPointer<AbstractModule>> createModulesList();
    QList<QSharedPointer<AbstractModule>> getModulesList();   

        void updateSpecGUI();
    Scan *scan;

private:
    QList<DeviceDescriptor> deviceList;
    QList<QSharedPointer<AbstractModule>> modules;

    Comms *communication;
    Commands *commands;
    DeviceSpec *deviceSpec;
    bool isConnected = false;

    bool getIsConnected() const;
    bool getIsSpecificationLoaded();
    DeviceSpec* getDeviceSpecification();

signals:
  //  void scopeNewData(QByteArray);
    void systemNewData(QByteArray);
    void writeData (QByteArray data);

    void updateDeviceList(QList<DeviceDescriptor> deviceList);
    void updateSpecfication();

private slots:
    void parseData(QByteArray data);
    void parseSystemData(QByteArray data);
    void newDeviceList(QList<DeviceDescriptor> deviceList);
    void handleError(QByteArray error);
    void ScanDevices();
    void open(int deviceIndex);
    void close();
};

#endif // DEVICE_H
