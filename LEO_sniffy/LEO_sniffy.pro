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
    GUI/clickablelabel.cpp \
    GUI/customcombobox.cpp \
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
    GUI/widgetlabelarea.cpp \
    GUI/widgetlist.cpp \
    GUI/widgetselection.cpp \
    GUI/widgetseparator.cpp \
    GUI/widgetswitch.cpp \
    GUI/widgettab.cpp \
    GUI/widgettextinput.cpp \
    communication/commands.cpp \
    communication/comms.cpp \
    communication/devicescanner.cpp \
    communication/serialline.cpp \
    communication/serialportreader.cpp \
    communication/serialportwriter.cpp \
    devicedescriptor.cpp \
    devicemediator.cpp \
    graphics/colors.cpp \
    main.cpp \
    mainwindow.cpp \
    math/movingaverage.cpp \
    math/timing.cpp \
    math/tinyexpr.c \
    modules/abstractconfiguration.cpp \
    modules/abstractmodule.cpp \
    modules/abstractspecification.cpp \
    modules/counter/counter.cpp \
    modules/counter/counterconfig.cpp \
    modules/counter/counterspec.cpp \
    modules/counter/countertabhighfreq.cpp \
    modules/counter/countertabintervals.cpp \
    modules/counter/countertablowfreq.cpp \
    modules/counter/countertabratio.cpp \
    modules/counter/counterwindow.cpp \
    modules/errorlist.cpp \
    modules/labelformator.cpp \
    modules/device/device.cpp \
    modules/device/devicewindow.cpp \
    modules/device/devicespec.cpp \
    modules/scope/mathcalculations.cpp \
    modules/scope/meascalculations.cpp \
    modules/scope/measurement.cpp \
    modules/scope/panelcursors.cpp \
    modules/scope/panelmath.cpp \
    modules/scope/panelmeasurement.cpp \
    modules/scope/panelsettings.cpp \
    modules/scope/scope.cpp \
    modules/scope/scopeconfig.cpp \
    modules/scope/scopespec.cpp \
    modules/scope/scopewindow.cpp \
    modules/syncpwm/syncpwm.cpp \
    modules/syncpwm/syncpwmconfig.cpp \
    modules/syncpwm/syncpwmspec.cpp \
    modules/syncpwm/syncpwmwindow.cpp \
    modules/template/templatemodule.cpp \
    modules/template/templatemoduleconfig.cpp \
    modules/template/templatemodulespec.cpp \
    modules/template/templatemodulewindow.cpp \
    modules/voltmeter/channeldata.cpp \
    modules/voltmeter/voltmeter.cpp \
    modules/voltmeter/voltmeterconfig.cpp \
    modules/voltmeter/voltmeterspec.cpp \
    modules/voltmeter/voltmeterwindow.cpp


HEADERS += \
    GUI/clickablelabel.h \
    GUI/customcombobox.h \
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
    GUI/widgetlabelarea.h \
    GUI/widgetlist.h \
    GUI/widgetselection.h \
    GUI/widgetseparator.h \
    GUI/widgetswitch.h \
    GUI/widgettab.h \
    GUI/widgettextinput.h \
    communication/commands.h \
    communication/comms.h \
    communication/connectiontype.h \
    communication/devicescanner.h \
    communication/serialline.h \
    communication/serialportreader.h \
    communication/serialportwriter.h \
    devicedescriptor.h \
    devicemediator.h \
    graphics/colors.h \
    graphics/styles.h \
    mainwindow.h \
    math/movingaverage.h \
    math/timing.h \
    math/tinyexpr.h \
    modules/abstractconfiguration.h \
    modules/abstractspecification.h \
    modules/counter/counter.h \
    modules/counter/counterconfig.h \
    modules/counter/counterdefs.h \
    modules/counter/counterspec.h \
    modules/abstractmodule.h \
    modules/counter/countertabhighfreq.h \
    modules/counter/countertabintervals.h \
    modules/counter/countertablowfreq.h \
    modules/counter/countertabratio.h \
    modules/counter/counterwindow.h \
    modules/errorlist.h \
    modules/labelformator.h \
    modules/device/device.h \
    modules/device/devicewindow.h \
    modules/device/devicespec.h \
    modules/scope/mathcalculations.h \
    modules/scope/meascalculations.h \
    modules/scope/measurement.h \
    modules/scope/panelcursors.h \
    modules/scope/panelmath.h \
    modules/scope/panelmeasurement.h \
    modules/scope/panelsettings.h \
    modules/scope/scope.h \
    modules/scope/scopeconfig.h \
    modules/scope/scopedefs.h \
    modules/scope/scopespec.h \
    modules/scope/scopewindow.h \
    modules/syncpwm/syncpwm.h \
    modules/syncpwm/syncpwmconfig.h \
    modules/syncpwm/syncpwmdefs.h \
    modules/syncpwm/syncpwmspec.h \
    modules/syncpwm/syncpwmwindow.h \
    modules/template/templatemodule.h \
    modules/template/templatemoduleconfig.h \
    modules/template/templatemodulespec.h \
    modules/template/templatemodulewindow.h \
    modules/voltmeter/channeldata.h \
    modules/voltmeter/voltmeter.h \
    modules/voltmeter/voltmeterconfig.h \
    modules/voltmeter/voltmeterdefs.h \
    modules/voltmeter/voltmeterspec.h \
    modules/voltmeter/voltmeterwindow.h



FORMS += \
    GUI/widgetbuttons.ui \
    GUI/widgetchart.ui \
    GUI/widgetdial.ui \
    GUI/widgetdialrange.ui \
    GUI/widgetdisplay.ui \
    GUI/widgetfooter.ui \
    GUI/widgetlabel.ui \
    GUI/widgetlabelarea.ui \
    GUI/widgetlist.ui \
    GUI/widgetmodule.ui \
    GUI/widgetselection.ui \
    GUI/widgetseparator.ui \
    GUI/widgetswitch.ui \
    GUI/widgettab.ui \
    GUI/widgettextinput.ui \
    mainwindow.ui \
    modules/counter/counterwindow.ui \
    modules/device/devicewindow.ui \
    modules/scope/scopewindow.ui \
    modules/syncpwm/syncpwmwindow.ui \
    modules/template/templatemodulewindow.ui \
    modules/voltmeter/voltmeterwindow.ui


TRANSLATIONS += \
    LEO_sniffy_en_GB.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    graphics.qrc
