#include "comms.h"

Comms::Comms(QObject *parent) : QObject(parent)
{

}

QList<device_descriptor> * Comms::scanForDevices(){

    listDevices = new QList<device_descriptor>;

    SerialLine::getAvailableDevices(listDevices,0);

    return  listDevices;
}

bool Comms::open(int deviceIndex){
    bool ret = false;

    switch (listDevices->at(deviceIndex).connType) {
    case Connection::SERIAL:
         serial = new SerialLine();
         ret = serial->openLine(listDevices->at(deviceIndex));
        break;
    default:
        break;
    }

    return ret;
}
