#ifndef SCAN_H
#define SCAN_H

#include <QObject>

#include "devicewindow.h"
#include "devicespec.h"

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/moduledockwidget.h"
#include "../abstractmodule.h"
#include "../errorlist.h"

class Device : public AbstractModule
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr, QByteArray configString = nullptr,Comms *communication = nullptr,WidgetControlModule *WidgetModule = nullptr,ModuleDockWidget *dockWidget= nullptr);

    QWidget* getWidget();
    QString getName();

    void updateGUIDeviceList(QList<DeviceDescriptor> deviceList);
    void errorHandler(QByteArray error);

private:
    void connectDevice(int index);
    void disconnectDevice();

    DeviceSpec *deviceSpec;
    DeviceWindow *deviceWindow;

signals:
    void ScanDevices();
    void openDevice(int deviceIndex);
    void closeDevice();

public slots:
    void parseData(QByteArray);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();

private slots:
    void deviceConnection(int buttonIndex);
};

#endif // SCAN_H
