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
    this->isSearchEnaled = isSearchEnaled;
}

void DeviceScanner::run()
{
    //qDebug () << "Thread scanning";
    isRunning = true;
    QList<DeviceDescriptor> tempDeviceList = *new QList<DeviceDescriptor>;
    while(isRunning){
        if(isSearchEnaled){
            tempDeviceList.clear();
            SerialLine::getAvailableDevices(&tempDeviceList,0);
            QDateTime date = QDateTime::currentDateTime();
            //qDebug() << date.time()<<"scanning" << isSearchEnaled <<"("<<currentDeviceList.length() << tempDeviceList.length()<<")";

            if(!deviceListsEqual(tempDeviceList,currentDeviceList)){
                currentDeviceList = tempDeviceList;
               // qDebug() << "new devices found"<<currentDeviceList.length();
               // if(currentDeviceList.length()>0){
                   // qDebug () << currentDeviceList.first();
               // }
                emit newDevicesScanned(currentDeviceList);
            }
        }else{
            thread()->msleep(500);
        }
        thread()->msleep(250);
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
    isRunning = false;
}


