#ifndef SERIALLINE_H
#define SERIALLINE_H

#include <QObject>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include "connectiontype.h"




class SerialLine : public QObject
{
    Q_OBJECT
public:
    explicit SerialLine(QObject *parent = nullptr);

    static int getAvailableDevices(QList<device_descriptor> *list, int setFirstIndex);

    bool openLine(device_descriptor desc);

signals:

private slots:
        void handleError(void);
        void receiveData(void);

private:

    //QSerialPortInfo *portInfo;
    QSerialPort *serPort;


};

#endif // SERIALLINE_H
