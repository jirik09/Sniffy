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

        QDataStream streamUID(specParams[3]);
        streamUID>>UID1;
        streamUID>>UID2;
        streamUID>>UID3;

        FW_Version = specParams[5];
        FREE_RTOS_Version = specParams[7];
        HAL_Version = specParams[9];

        QDataStream streamBuffLeng(specParams[11]);
        streamBuffLeng>>bufferLength;

        QDataStream streamUARTSpeed(specParams[12]);
        streamUARTSpeed>>uartSpeed;

        TX_pin = specParams[13];
        RX_pin = specParams[14];

        if(specParams.length()>=18){
            useUsb = true;
            DP_pin = specParams[16];
            DM_pin = specParams[17];
        }
        isSpecLoaded = true;
    }
}
