#ifndef SCAN_H
#define SCAN_H

#include <QObject>

#include "devicewindow.h"
#include "devicespec.h"

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/moduledockwidget.h"
#include "../abstractmodule.h"

class Device : public AbstractModule
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);

    QWidget* getWidget();

    void updateGUIDeviceList(QList<DeviceDescriptor> deviceList);
    void errorHandler(QByteArray error);

private:
    void connectDevice(int index);
    void disconnectDevice();

    DeviceSpec *deviceSpec;
    DeviceWindow *deviceWindow;

signals:
    void ScanDevices();
    void opened(int deviceIndex);
    void closed();

public slots:
    void parseData(QByteArray);
    void writeConfiguration();
    void startModule();
    void stopModule();

private slots:
    void deviceConnection(int buttonIndex);
};

#endif // SCAN_H
