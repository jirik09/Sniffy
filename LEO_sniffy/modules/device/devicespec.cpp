#include "devicespec.h"

namespace {

QString normalizeVersionString(QString version)
{
    if (version.isEmpty()) {
        return version;
    }

    if (version.startsWith(QLatin1Char('V'))) {
        version[0] = QLatin1Char('v');
        return version;
    }

    if (!version.startsWith(QLatin1Char('v'))) {
        version.prepend(QLatin1Char('v'));
    }

    return version;
}

} // namespace

DeviceSpec::DeviceSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void DeviceSpec::parseSpecification(QByteArray spec){
    QList<QByteArray> specParams = spec.split(':');

    if(specParams.length()>=15){
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

        FW_Version = normalizeVersionString(QString::fromUtf8(specParams[7]));
        OS_Version = specParams[9];

        QDataStream streamBuffLeng(specParams[11]);
        streamBuffLeng>>bufferLength;

        QDataStream streamUARTSpeed(specParams[12]);
        streamUARTSpeed>>uartSpeed;

        TX_pin = specParams[13];
        RX_pin = specParams[14];

        if(specParams.length()>=18 && specParams[15] == "USB_"){
            useUsb = true;
            DP_pin = specParams[16];
            DM_pin = specParams[17];
        }
        isSpecificationLoaded = true;
    }
}
