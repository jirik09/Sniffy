#include "mainwindow.h"

#include <QSerialPort>

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "devicedescriptor.h"
#include "modules/scope/measurement.h"
#include "customsettings.h"

int main(int argc, char *argv[])
{
    // Qt6: QTextCodec setCodecForLocale removed (UTF-8 is default). Removed obsolete call.

    QApplication app(argc, argv);
    app.setOrganizationName("LEO Sniffy");
    app.setApplicationName("LEO Sniffy");
    app.setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));

    qRegisterMetaType<QList<Measurement *>>();
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(configPath);
    }
    CustomSettings::loadSettings(configPath + "/settings.ini");
    CustomSettings::setThemesList(Graphics::initThemesList());

    // Set theme explicitly (replaces deprecated getThemeInstance(int) usage)
    Graphics::setTheme(CustomSettings::getThemeIndex());
    if (auto themePtr = Graphics::getThemeInstance()) {
        app.setStyleSheet(themePtr->getAppGlobalStyle());
    }

    MainWindow w;
    w.show();
    return app.exec();
}
