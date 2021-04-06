#include "mainwindow.h"

#include <QApplication>
#include "devicedescriptor.h"
#include "modules/scope/measurement.h"
#include "customsettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QList<Measurement *>>();
    qRegisterMetaType<QList<DeviceDescriptor>>();
    qRegisterMetaType<QSerialPort::SerialPortError>();

    QString styleSheet = STYLESH_GLOBAL;
    a.setStyleSheet(styleSheet);

    CustomSettings::loadSettings(QApplication::applicationDirPath() + "/settings.ini");

    QList<AbstractColors*> *themes;
    themes = new QList<AbstractColors*>;
    themes->append(new DarkTheme());
    themes->append(new LightTheme());

    for(int i = 0;i<themes->length();i++){
        CustomSettings::addTheme(themes->at(i)->getThemeName());
    }

    Colors(nullptr, themes->at(CustomSettings::getThemeIndex()));

    MainWindow w;
    w.show();
    return a.exec();
}
