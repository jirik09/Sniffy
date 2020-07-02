#ifndef SCAN_H
#define SCAN_H

#include <QObject>

#include "devicewindow.h"

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/moduledockwidget.h"
#include "../abstractmodule.h"

class Device : public AbstractModule
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);

    QWidget* getWidget();

    DeviceWindow *scanWindow;

    void updateGUIDeviceList(QList<DeviceDescriptor> deviceList);
    void errorHandler(QByteArray error);

private:
    void connectDevice(int index);
    void disconnectDevice();

signals:
    void ScanDevicesGUI();
    void openGUI(int deviceIndex);
    void closeGUI();
    bool getIsConnectedGUI() const;
    void updateSpecGUIGUI();
    //bool getIsSpecificationLoadedGUI();
    //void getDeviceSpecificationGUI(void *devSpec);

public slots:
    void parseData(QByteArray);
    void writeConfiguration();
    void startModule();
    void stopModule();

private slots:
    //void writeVerticalLayoutSpec(QVBoxLayout *boxLayout);
    void deviceConnection(int buttonIndex);
};

#endif // SCAN_H
