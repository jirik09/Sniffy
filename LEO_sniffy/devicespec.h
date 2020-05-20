#ifndef DEVICESPEC_H
#define DEVICESPEC_H

#include <QObject>

class DeviceSpec : public QObject
{
    Q_OBJECT
public:
    explicit DeviceSpec(QObject *parent = nullptr);

signals:

public:

    uint32_t CoreClock;
    uint32_t PeriphClock;
    QString MCU;
    QString FW_Version;
    QString FREE_RTOS_Version;
    QString HAL_Version;
    bool isShield = false;
    uint32_t VDDA;

    uint32_t bufferLength;
    uint32_t UartSpeed;
    QString TX_pin;
    QString RX_pin;
    bool useUsb = false;
    QString DP_pin;
    QString DM_pin;

};

#endif // DEVICESPEC_H
