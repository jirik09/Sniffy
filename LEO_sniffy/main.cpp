#include "mainwindow.h"

#include <QApplication>
#include "devicedescriptor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();
    MainWindow w;
    w.show();
    return a.exec();
}
