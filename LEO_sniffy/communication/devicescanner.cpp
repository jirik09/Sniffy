#include "devicescanner.h"

DeviceScanner::DeviceScanner(QObject *parent) : QThread(parent)
{
}

DeviceScanner::~DeviceScanner()
{
    isRunning = false;
}

void DeviceScanner::searchForDevices(bool isSearchEnaled)
{
    this->isSearchEnaled.store(isSearchEnaled, std::memory_order_relaxed);
}

void DeviceScanner::run()
{
    isRunning = true;
    QList<DeviceDescriptor> tempDeviceList;
    while(isRunning.load(std::memory_order_relaxed)){
        if(isSearchEnaled.load(std::memory_order_relaxed)){
            tempDeviceList.clear();
            SerialLine::getAvailableDevices(&tempDeviceList,0);

            if(!deviceListsEqual(tempDeviceList,currentDeviceList)){
                currentDeviceList = tempDeviceList;
               // qDebug() << "new devices found"<<currentDeviceList.length();
                isSearchEnaled.store(false, std::memory_order_relaxed);
                emit newDevicesScanned(currentDeviceList);
            }
            // backoff a little to avoid hammering the system
            QThread::msleep(200);
        }else{
            QThread::msleep(1500);
        }
        QThread::msleep(800);
    }
}

bool DeviceScanner::deviceListsEqual(QList<DeviceDescriptor> &listA, QList<DeviceDescriptor> &listB)
{
    bool equal = false;
    if(listB.length() == listA.length()){
        equal = true;
        for(int i = 0;i<listA.length();i++){
            if(listA.at(i).deviceName != listB.at(i).deviceName || listA.at(i).port != listB.at(i).port){
                equal = false;
                break;
            }
        }
    }
    return equal;
}

void DeviceScanner::quit()
{
    isRunning.store(false, std::memory_order_relaxed);
}


