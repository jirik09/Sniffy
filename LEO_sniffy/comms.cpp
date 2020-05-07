#include "comms.h"

Comms::Comms(QObject *parent) : QObject(parent)
{

}

QList<device_descriptor>* Comms::scanForDevices(){

    //listDevices = new QList<device_descriptor>();
    //ser = new Serial();
  //  Serial::getAvailableDevices(listDevices,0);

    return  listDevices;
}
