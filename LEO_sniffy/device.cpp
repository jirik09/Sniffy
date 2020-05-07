#include "device.h"

Device::Device(QObject *parent) : QObject(parent)
{

}

QList<device_descriptor> Device::scan (){

    QList<device_descriptor> list;
    list = *comunication->scanForDevices();
    return list;
}
