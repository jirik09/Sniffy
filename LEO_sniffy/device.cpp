#include "device.h"

Device::Device(QObject *parent, Comms *comm) : QObject(parent)
{
    communication = comm;
    scope = new Scope();
}


void Device::open(int deviceIndex){
    isConnected = communication->open(deviceIndex);
    if(isConnected){
        connect(communication,SIGNAL(newData(QByteArray)),this,SLOT(parseData(QByteArray)));

        connect(this,SIGNAL(scopeNewData(QByteArray)),scope,SLOT(parseData(QByteArray)));
        connect(this,SIGNAL(systemNewData(QByteArray)),this, SLOT(parseSystemData(QByteArray)));

    }
}

void Device::write(const char *data){
    communication->write(data);
}

void Device::loadHWSpecification(void){

    communication->write(commands.RESET_DEVICE+";");
    QThread::msleep(100);

    communication->write(commands.VERSION_REQUEST+";");
    communication->write(commands.IS_SHIELD_CONNECTED+";");
    communication->write(commands.SYSTEM+":"+commands.CONFIG_REQUEST+";");
    communication->write(commands.COMMS+":"+commands.CONFIG_REQUEST+";");
    communication->write(commands.SCOPE+":"+commands.CONFIG_REQUEST+";");

}

void Device::parseData(QByteArray data){

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    if(dataHeader=="OSCP"){
       emit scopeNewData(dataToPass);
    }else if(dataHeader=="SYST"){
       emit systemNewData(dataToPass);
    }else{
       qDebug() << "ERROR: this data could not be passed into module" << data;
    }
}

void Device::parseSystemData(QByteArray data){
    qDebug() << "data are in system parser" << data;

    QByteArray feature = data.left(4);
    data.remove(0,4);
    QDataStream dataStream(data);
    QByteArray tmp;

    if(feature=="VER_"){
       specification.FW_Version = data.mid(6,8);
       specification.FREE_RTOS_Version = data.mid(22,6);
       specification.HAL_Version = data.mid(34,6);
    }else if(feature=="SYST"){
        dataStream >> specification.CoreClock;
        dataStream >> specification.PeriphClock;
        specification.MCU = data.right(data.length()-8);
    }else if(feature=="COMM"){
        dataStream >> specification.bufferLength;
        dataStream >> specification.UartSpeed;
        specification.TX_pin = data.mid(8,4);
        specification.RX_pin = data.mid(12,4);
        if(data.length()>20){
            specification.useUsb = true;
            specification.DP_pin = data.mid(20,4);
            specification.DM_pin = data.mid(24,4);
        }
    }else if(feature=="ACK_"){
        //do nothing
    }else{
        qDebug() << "ERROR: unparsable data for system" << data;

    }

}



