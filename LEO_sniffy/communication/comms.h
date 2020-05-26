#ifndef COMMS_H
#define COMMS_H

#include <QObject>
#include "connectiontype.h"
#include "serialline.h"

class Comms : public QObject
{
    Q_OBJECT
public:
    explicit Comms(QObject *parent = nullptr);
    QList<device_descriptor> * scanForDevices();
    bool open(int deviceIndex);
    void close();

    void write(const char *data);
    void write(QByteArray data);
    void write(QString data);
    void writeUint32(int data);
    void writeUint16(int data);


signals:
        void newData(QByteArray message);

private slots:
    void parseMessage(QByteArray message);

private:
    QList<device_descriptor> *listDevices;
    SerialLine *serial;

};

#endif // COMMS_H
