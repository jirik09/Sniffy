QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    GUI/widgetbuttons.cpp \
    GUI/widgetdial.cpp \
    GUI/widgetdialrange.cpp \
    GUI/widgetfeature.cpp \
    GUI/widgetfooter.cpp \
    GUI/widgetlabel.cpp \
    GUI/widgetselection.cpp \
    GUI/widgetseparator.cpp \
    GUI/widgetswitch.cpp \
    GUI/widgettop.cpp \
    GUI/windowscope.cpp \
    communication/comms.cpp \
    communication/serialline.cpp \
    device.cpp \
    features/scope/scope.cpp \
    features/scope/scopeconfig.cpp \
    features/scope/scopespec.cpp \
    main.cpp \
    mainwindow.cpp \


HEADERS += \
    GUI/customdial.h \
    GUI/widgetbuttons.h \
    GUI/widgetdial.h \
    GUI/widgetdialrange.h \
    GUI/widgetfeature.h \
    GUI/widgetfooter.h \
    GUI/widgetlabel.h \
    GUI/widgetselection.h \
    GUI/widgetseparator.h \
    GUI/widgetswitch.h \
    GUI/widgettop.h \
    GUI/windowscope.h \
    communication/comms.h \
    communication/connectiontype.h \
    communication/serialline.h \
    device.h \
    features/scope/scope.h \
    features/scope/scopeconfig.h \
    features/scope/scopedefs.h \
    features/scope/scopespec.h \
    graphics/colors.h \
    mainwindow.h \



FORMS += \
    GUI/widgetbuttons.ui \
    GUI/widgetdial.ui \
    GUI/widgetdialrange.ui \
    GUI/widgetfeature.ui \
    GUI/widgetfooter.ui \
    GUI/widgetlabel.ui \
    GUI/widgetselection.ui \
    GUI/widgetseparator.ui \
    GUI/widgetswitch.ui \
    GUI/widgettop.ui \
    GUI/windowscope.ui \
    mainwindow.ui \


TRANSLATIONS += \
    LEO_sniffy_en_GB.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    graphics.qrc
