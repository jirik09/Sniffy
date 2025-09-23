#ifndef DEVICESCANNER_H
#define DEVICESCANNER_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include <atomic>

#include "devicedescriptor.h"
#include "serialLine.h"

class DeviceScanner : public QThread
{
    Q_OBJECT
public:
    explicit DeviceScanner(QObject *parent = nullptr);
    ~DeviceScanner();
    void searchForDevices(bool isSearchEnaled);

signals:
    void newDevicesScanned(QList<DeviceDescriptor> deviceList);


private:
    void run() override;
    bool deviceListsEqual(QList<DeviceDescriptor> &listA, QList<DeviceDescriptor> &listB);
    std::atomic<bool> isSearchEnaled { false };
    std::atomic<bool> isRunning { true };
    QList<DeviceDescriptor> currentDeviceList;
    SerialLine serLine;

public slots:
    void quit();
};

#endif // DEVICESCANNER_H
