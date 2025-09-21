#include "mainwindow.h"

#include <QSerialPort>

#include <QApplication>
#include "devicedescriptor.h"
#include "modules/scope/measurement.h"
#include "customsettings.h"

int main(int argc, char *argv[])
{    
    // Qt6: QTextCodec setCodecForLocale removed (UTF-8 is default). Removed obsolete call.

    QApplication app(argc, argv);
    qRegisterMetaType<QList<Measurement *>>();
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();

    CustomSettings::loadSettings(QApplication::applicationDirPath() + "/settings.ini");
    CustomSettings::setThemesList(Graphics::initThemesList());

    //Graphics(nullptr, CustomSettings::getThemeIndex());
    app.setStyleSheet(Graphics::getThemeInstance(CustomSettings::getThemeIndex())->getAppGlobalStyle());

    MainWindow w;
    w.show();
    return app.exec();
}
