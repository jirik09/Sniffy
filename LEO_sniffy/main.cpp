#include "mainwindow.h"

#include <QTextCodec>

#include <QApplication>
#include "devicedescriptor.h"
#include "modules/scope/measurement.h"
#include "customsettings.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);
    qRegisterMetaType<QList<Measurement *>>();
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();

    CustomSettings::loadSettings(QApplication::applicationDirPath() + "/settings.ini");

    QList<AbstractGraphics*> *themes;
    themes = new QList<AbstractGraphics*>;
    themes->append(new DarkTheme());
    themes->append(new LightTheme());

    for(int i = 0;i<themes->length();i++){
        CustomSettings::addTheme(themes->at(i)->getThemeName());
    }

    int themeIndex = CustomSettings::getThemeIndex();
    Graphics(nullptr, themes->at(themeIndex));
    a.setStyleSheet(themes->at(themeIndex)->getAppGlobalStyle());

    MainWindow w;
    w.show();
    return a.exec();
}
