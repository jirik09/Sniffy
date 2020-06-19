QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QT += charts

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GUI/customdial.cpp \
    GUI/moduledockwidget.cpp \
    GUI/widgetbuttons.cpp \
    GUI/widgetchart.cpp \
    GUI/widgetcontrolmodule.cpp \
    GUI/widgetdial.cpp \
    GUI/widgetdialrange.cpp \
    GUI/widgetdisplay.cpp \
    GUI/widgetfooter.cpp \
    GUI/widgetlabel.cpp \
    GUI/widgetselection.cpp \
    GUI/widgetseparator.cpp \
    GUI/widgetswitch.cpp \
    GUI/widgettab.cpp \
    GUI/widgettop.cpp \
    communication/comms.cpp \
    communication/serialline.cpp \
    communication/serialportreader.cpp \
    communication/serialportwriter.cpp \
    device.cpp \
    devicedescriptor.cpp \
    devicespec.cpp \
    main.cpp \
    mainwindow.cpp \
    modules/abstractmodule.cpp \
    modules/counter/counter.cpp \
    modules/counter/counterconfig.cpp \
    modules/counter/counterspec.cpp \
    modules/counter/windowcounter.cpp \
    modules/scope/scope.cpp \
    modules/scope/scopeconfig.cpp \
    modules/scope/scopespec.cpp \
    modules/scope/windowscope.cpp


HEADERS += \
    GUI/customdial.h \
    GUI/moduledockwidget.h \
    GUI/widgetbuttons.h \
    GUI/widgetchart.h \
    GUI/widgetcontrolmodule.h \
    GUI/widgetdial.h \
    GUI/widgetdialrange.h \
    GUI/widgetdisplay.h \
    GUI/widgetfooter.h \
    GUI/widgetlabel.h \
    GUI/widgetselection.h \
    GUI/widgetseparator.h \
    GUI/widgetswitch.h \
    GUI/widgettab.h \
    GUI/widgettop.h \
    communication/commands.h \
    communication/comms.h \
    communication/connectiontype.h \
    communication/serialline.h \
    communication/serialportreader.h \
    communication/serialportwriter.h \
    device.h \
    devicedescriptor.h \
    devicespec.h \
    graphics/colors.h \
    mainwindow.h \
    modules/counter/counter.h \
    modules/counter/counterconfig.h \
    modules/counter/counterdefs.h \
    modules/counter/counterspec.h \
    modules/abstractmodule.h \
    modules/counter/windowcounter.h \
    modules/scope/scope.h \
    modules/scope/scopeconfig.h \
    modules/scope/scopedefs.h \
    modules/scope/scopespec.h \
    modules/scope/windowscope.h



FORMS += \
    GUI/widgetbuttons.ui \
    GUI/widgetchart.ui \
    GUI/widgetdial.ui \
    GUI/widgetdialrange.ui \
    GUI/widgetdisplay.ui \
    GUI/widgetfooter.ui \
    GUI/widgetlabel.ui \
    GUI/widgetmodule.ui \
    GUI/widgetselection.ui \
    GUI/widgetseparator.ui \
    GUI/widgetswitch.ui \
    GUI/widgettab.ui \
    GUI/widgettop.ui \
    mainwindow.ui \
    modules/counter/windowcounter.ui \
    modules/scope/windowscope.ui


TRANSLATIONS += \
    LEO_sniffy_en_GB.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    graphics.qrc
