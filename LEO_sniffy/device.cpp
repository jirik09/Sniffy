#include "device.h"

Device::Device(QObject *parent, Comms *comm, int deviceToOpen) : QObject(parent)
{
    communication = comm;
    if(deviceToOpen!=-1){
        connected = connect(deviceToOpen);
    }

    scope = new Scope();
}


bool Device::connect(int PortIndex){
    return communication->open(PortIndex);
}

void Device::loadSpecification(void){


}

void Device::parseMessage(){



}



