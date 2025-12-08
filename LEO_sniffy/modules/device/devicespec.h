#ifndef DEVICESPEC_H
#define DEVICESPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class DeviceSpec : public AbstractSpecification
{
public:
    explicit DeviceSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    QString device;
    QString MCU;
    uint32_t CoreClock;

    QString MCU_ID;
    QString Build_Date;
    QString FW_Version;
    QString OS_Version;
    QString HAL_Version;

    bool isShield = false;
    QString shieldName;

    uint32_t bufferLength;
    uint32_t uartSpeed;
    QString TX_pin;
    QString RX_pin;
    bool useUsb = false;
    QString DP_pin;
    QString DM_pin;
};

#endif // DEVICESPEC_H
