#include "devicespec.h"

DeviceSpec::DeviceSpec(QObject *parent) : QObject(parent)
{

}

void DeviceSpec::parseSpecification(QByteArray spec){
    QList<QByteArray> specParams = spec.split(':');

    if(specParams.length()>=16){
        device = specParams[0];
        if(specParams[1] == "NACK"){
            isShield = false;
        }else{
            isShield = true;
            shieldName = specParams[1];
        }
        MCU = specParams[2];

        QDataStream streamCLK(specParams[3]);
        streamCLK>>CoreClock;

        QDataStream streamUID(specParams[4]);
        streamUID>>UID1;
        streamUID>>UID2;
        streamUID>>UID3;

        FW_Version = specParams[6];
        FREE_RTOS_Version = specParams[8];
        HAL_Version = specParams[10];

        QDataStream streamBuffLeng(specParams[12]);
        streamBuffLeng>>bufferLength;

        QDataStream streamUARTSpeed(specParams[13]);
        streamUARTSpeed>>uartSpeed;

        TX_pin = specParams[14];
        RX_pin = specParams[15];

        if(specParams.length()>=18){
            useUsb = true;
            DP_pin = specParams[17];
            DM_pin = specParams[18];
        }
        isSpecLoaded = true;
    }
}
