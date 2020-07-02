#include "mainwindow.h"

#include <QApplication>
#include "devicedescriptor.h"
#include "modules/scope/measurement.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QList<Measurement *>>();
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();
    MainWindow w;
    w.show();
    return a.exec();
}
