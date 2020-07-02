#ifndef DEVICE_H
#define DEVICE_H

//#include <QObject>
//#include <QThread>
//#include <QSharedPointer>
#include "communication/comms.h"
#include "communication/commands.h"
#include "modules/device/device.h"
#include "modules/scope/scope.h"


class DeviceMediator : public QObject
{
    Q_OBJECT
public:
    explicit DeviceMediator(QObject *parent = nullptr);
    QList<QSharedPointer<AbstractModule>> createModulesList();
    QList<QSharedPointer<AbstractModule>> getModulesList();   

        void updateSpecGUI();


private:
    QList<DeviceDescriptor> deviceList;
    QList<QSharedPointer<AbstractModule>> modules;
    Device *device;

    Comms *communication;

    bool isConnected = false;

    bool getIsConnected() const;

signals:
  //  void scopeNewData(QByteArray);
    void systemNewData(QByteArray);
    void writeData (QByteArray data);

    void updateDeviceList(QList<DeviceDescriptor> deviceList);
    void updateSpecfication();

private slots:
    void parseData(QByteArray data);
    void newDeviceList(QList<DeviceDescriptor> deviceList);
    void handleError(QByteArray error);
    void ScanDevices();
    void open(int deviceIndex);
    void close();
};

#endif // DEVICE_H
