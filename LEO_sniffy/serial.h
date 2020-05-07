#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include "connectiontype.h"




class Serial : public ConnectionType
{
    Q_OBJECT
public:
    explicit Serial();

    int getAvailableDevices(QList<device_descriptor> *list, int setFirstIndex);

signals:

private:

    QSerialPortInfo *portInfo;
    QSerialPort *serPort;

};

#endif // SERIAL_H
