#include "serialLine.h"

SerialLine::SerialLine(QObject *parent) : QObject(parent)
{
    buffer.reserve(8192);
    message.reserve(1024);
}

SerialLine::~SerialLine()
{
    closeLine();
    resetPort();
}

int SerialLine::getAvailableDevices(QList<DeviceDescriptor> *list, int setFirstIndex){

    const QByteArray delimiter = QByteArray::fromRawData(delimiterRaw,4);

    QSerialPort *sPort;
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    int numberOfDevices = 0;
    QByteArray received;

    for (const QSerialPortInfo &tmpPortInfo : ports){
        QSerialPortInfo portIn (tmpPortInfo.portName());

        sPort = new QSerialPort(portIn,nullptr);

        sPort->setBaudRate(921600);
        sPort->setDataBits(QSerialPort::DataBits::Data8);
        sPort->setParity(QSerialPort::Parity::NoParity);
        sPort->setStopBits(QSerialPort::StopBits::OneStop);

        if(sPort->open(QIODevice::ReadWrite)){
            sPort->clear();
            sPort->write("IDN?;");
            sPort->waitForBytesWritten(100);
            sPort->waitForReadyRead(150);
            QThread::msleep(250);

            received = sPort->readAll();

            if (received.length()>16 && received.left(4)=="SYST" && received.right(4)==delimiter){
                sPort->write("RES!;");
                sPort->waitForBytesWritten();

                DeviceDescriptor desc;
                desc.port = sPort->portName();
                desc.speed = sPort->baudRate();
                desc.connType = Connection::SERIAL;
                desc.index = setFirstIndex + numberOfDevices;
                desc.deviceName = received.mid(4, received.length()-8);
                list->append(desc);
                numberOfDevices++;
            }
            sPort->close();
        }
        delete sPort;
    }
    return numberOfDevices;
}

void SerialLine::openSerialLine(DeviceDescriptor desc){

    //qDebug()<< "serial line open"<<this->thread();
    bool success = false;
    resetPort();
    serPort = new QSerialPort();
    serPort->setPortName(desc.port);
    serPort->setBaudRate(desc.speed);

    buffer.clear();
    message.clear();

    int i = 0;
    while(!serPort->open(QIODevice::ReadWrite)){
        QThread::msleep(200);
        i++;
        if(i>25){
            break;
        }
         qDebug() << "ERROR wait for serport to be opened";
    }
    if(serPort->isOpen()){
        success=true;
    serialReader = new SerialPortReader(serPort,this);
    serialWriter = new SerialPortWriter(serPort,this);

        connect(serialReader,&SerialPortReader::newData, this, &SerialLine::receiveData);
        connect(serPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    }else{
        qDebug () << "Serial port error opening";
        handleError(QSerialPort::OpenError);
    }

    isOpen=success;
    emit connectionOpened(success);
}

void SerialLine::closeLine(){
    if (serPort){
        serPort->flush();
        serPort->close();
    }
    isOpen = false;
}

void SerialLine::handleError(QSerialPort::SerialPortError error){
    qDebug()<<"FATAL ERROR occured on serial line" << error;
    emit serialLineError("SerialLine Error");
}


void SerialLine::receiveData(QByteArray data){
    buffer.append(data);
    // find delimiter(s); allow delimiter at start (should not happen normally) and handle consecutive messages
    int idx = buffer.indexOf(delimiter);
    while(idx >= 0){
        if(idx > 0){
            message = buffer.left(idx);
            emit newMessage(message);
        }
        // remove processed part + delimiter
        buffer.remove(0, idx + delimiter.size());
        idx = buffer.indexOf(delimiter);
    }
}

void SerialLine::resetPort()
{
    if (serPort){
        if(serPort->isOpen()){
            serPort->flush();
            serPort->close();
        }
        delete serPort;
        serPort = nullptr;
    }
    serialReader = nullptr; // QObject children auto-deleted
    serialWriter = nullptr;
}

bool SerialLine::getIsOpen() const
{
    return isOpen;
}

void SerialLine::write(const QByteArray &writeData){
    QDateTime date = QDateTime::currentDateTime();
    //qDebug() << "Sent" <<  date.time() <<":" << writeData;
    if(serPort->isOpen()){
        serialWriter->write(writeData);
    }else{
        qDebug() << "ERROR port is closed. Data cannot be send ("<<writeData<<")";
    }
}


