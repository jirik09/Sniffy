#ifndef DEVICESPEC_H
#define DEVICESPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"

class DeviceSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit DeviceSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    QString device;
    QString MCU;
    uint32_t CoreClock;

    uint32_t UID1;
    uint32_t UID2;
    uint32_t UID3;

    QString MCU_ID;
    QString Build_Date;
    QString FW_Version;
    QString FREE_RTOS_Version;
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
