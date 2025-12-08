#include "devicespec.h"

DeviceSpec::DeviceSpec(QObject *parent)
{
    Q_UNUSED(parent);
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

        // Convert the raw MCU_ID data to hex string
        MCU_ID = specParams[4].toHex().toUpper();

        Build_Date = specParams[5];

        FW_Version = specParams[7];
        OS_Version = specParams[9];
        HAL_Version = specParams[11];

        QDataStream streamBuffLeng(specParams[13]);
        streamBuffLeng>>bufferLength;

        QDataStream streamUARTSpeed(specParams[14]);
        streamUARTSpeed>>uartSpeed;

        TX_pin = specParams[15];
        RX_pin = specParams[16];

        if(specParams.length()>=19){
            useUsb = true;
            DP_pin = specParams[18];
            DM_pin = specParams[19];
        }
        isSpecificationLoaded = true;
    }
}
