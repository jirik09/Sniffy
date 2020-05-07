#include "serial.h"

Serial::Serial() : ConnectionType(parent())
{

}

int Serial::getAvailableDevices(QList<device_descriptor> *list, int setFirstIndex){

    QSerialPortInfo *portInfo = new QSerialPortInfo();
    QSerialPort *sPort;

    QList<QSerialPortInfo> ports;
    ports = portInfo->availablePorts();

    QSerialPortInfo tmpPort;
    int numberOfDevices = 0;
    QByteArray received;
    device_descriptor desc;
    foreach (tmpPort, ports){
        sPort = new QSerialPort(tmpPort);
        sPort->setBaudRate(921600);
        if(sPort->open(QIODevice::ReadWrite)){
            sPort->write("IDN?;");
            QThread::msleep(50);
            received = sPort->readAll();
            if (received.length()>1){
                desc.port = tmpPort.portName();
                desc.speed = 921600;
                desc.connType = Connection::SERIAL;
                desc.index =setFirstIndex + numberOfDevices;
                desc.deviceName = received;
                list->append(desc);
                numberOfDevices++;
            }
        }

    }
    return numberOfDevices;
}
