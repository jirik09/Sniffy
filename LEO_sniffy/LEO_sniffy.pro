QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

QT += charts
QT += openglwidgets
QT += network

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# Enforce modern range-based for usage (all legacy foreach removed)
DEFINES += QT_NO_FOREACH

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GUI/clickablelabel.cpp \
    GUI/customcombobox.cpp \
    GUI/customdial.cpp \
    GUI/moduledockwidget.cpp \
    GUI/qcustomplot.cpp \
    GUI/widgetbuttons.cpp \
    GUI/widgetchart.cpp \
    GUI/widgetcontrolmodule.cpp \
    GUI/widgetcustomplot.cpp \
    GUI/widgetdesciptionexpand.cpp \
    GUI/widgetdial.cpp \
    GUI/widgetdialrange.cpp \
    GUI/widgetdisplay.cpp \
    GUI/widgetfooter.cpp \
    GUI/widgetlabel.cpp \
    GUI/widgetlabelarea.cpp \
    GUI/widgetlist.cpp \
    GUI/widgetlogininfo.cpp \
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
    customsettings.cpp \
    devicedescriptor.cpp \
    devicemediator.cpp \
    graphics/abstracttheme.cpp \
    graphics/graphics.cpp \
    graphics/themes/dark.cpp \
    graphics/themes/dawn.cpp \
    graphics/themes/aurora.cpp \
    graphics/themes/neomorph.cpp \
    graphics/themes/sunset.cpp \
    graphics/themes/tealanalytics.cpp \
    graphics/themes/hudcyan.cpp \
    graphics/themes/light.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    math/movingaverage.cpp \
    math/timing.cpp \
    math/tinyexpr.c \
    modules/abstractconfiguration.cpp \
    modules/abstractmodule.cpp \
    modules/abstractspecification.cpp \
    modules/arbgenerator/arbgenerator.cpp \
    modules/arbgenerator/arbgeneratorconfig.cpp \
    modules/arbgenerator/arbgeneratorfileloader.cpp \
    modules/arbgenerator/arbgeneratorspec.cpp \
    modules/arbgenerator/arbgeneratorwindow.cpp \
    modules/arbgenerator/arbgenpanelsettings.cpp \
    modules/arbgenerator/arbgensweepcontroller.cpp \
    modules/arbgenerator/gencommons.cpp \
    modules/arbgenerator/signalcreator.cpp \
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
    modules/numberparser.cpp \
    modules/patterngenerator/patterngenerator.cpp \
    modules/patterngenerator/patterngeneratorconfig.cpp \
    modules/patterngenerator/patterngeneratorpainter.cpp \
    modules/patterngenerator/patterngeneratorpatterns.cpp \
    modules/patterngenerator/patterngeneratorsettings.cpp \
    modules/patterngenerator/patterngeneratorspec.cpp \
    modules/patterngenerator/patterngeneratorwindow.cpp \
    modules/pwmgenerator/pwmgeneratorconfig.cpp \
    modules/pwmgenerator/pwmgeneratorspec.cpp \
    modules/pwmgenerator/pwmgeneratorwindow.cpp \
    modules/pwmgenerator/pwngenerator.cpp \
    modules/scope/fftengine.cpp \
    modules/scope/mathcalculations.cpp \
    modules/scope/meascalculations.cpp \
    modules/scope/measurement.cpp \
    modules/scope/paneladvanced.cpp \
    modules/scope/panelcursors.cpp \
    modules/scope/panelmath.cpp \
    modules/scope/panelmeasurement.cpp \
    modules/scope/panelsettings.cpp \
    modules/scope/scope.cpp \
    modules/scope/scopeconfig.cpp \
    modules/scope/scopespec.cpp \
    modules/scope/scopewindow.cpp \
    modules/scope/timebaseandmemory.cpp \
    modules/syncpwm/syncpwm.cpp \
    modules/syncpwm/syncpwmconfig.cpp \
    modules/syncpwm/syncpwmpainter.cpp \
    modules/syncpwm/syncpwmsettings.cpp \
    modules/syncpwm/syncpwmspec.cpp \
    modules/syncpwm/syncpwmwindow.cpp \
    modules/template/templatemodule.cpp \
    modules/template/templatemoduleconfig.cpp \
    modules/template/templatemodulespec.cpp \
    modules/template/templatemodulewindow.cpp \
    modules/voltagesource/voltagesource.cpp \
    modules/voltagesource/voltagesourceconfig.cpp \
    modules/voltagesource/voltagesourcespec.cpp \
    modules/voltagesource/voltagesourcewindow.cpp \
    modules/voltmeter/channeldata.cpp \
    modules/voltmeter/voltmeter.cpp \
    modules/voltmeter/voltmeterconfig.cpp \
    modules/voltmeter/voltmeterspec.cpp \
    modules/voltmeter/voltmeterwindow.cpp \
    settingsdialog.cpp


HEADERS += \
    GUI/clickablelabel.h \
    GUI/customcombobox.h \
    GUI/customdial.h \
    GUI/moduledockwidget.h \
    GUI/qcustomplot.h \
    GUI/widgetbuttons.h \
    GUI/widgetchart.h \
    GUI/widgetcontrolmodule.h \
    GUI/widgetcustomplot.h \
    GUI/widgetdesciptionexpand.h \
    GUI/widgetdial.h \
    GUI/widgetdialrange.h \
    GUI/widgetdisplay.h \
    GUI/widgetfooter.h \
    GUI/widgetlabel.h \
    GUI/widgetlabelarea.h \
    GUI/widgetlist.h \
    GUI/widgetlogininfo.h \
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
    customsettings.h \
    devicedescriptor.h \
    devicemediator.h \
    graphics/abstracttheme.h \
    graphics/graphics.h \
    graphics/themes/dark.h \
    graphics/themes/dawn.h \
    graphics/themes/aurora.h \
    graphics/themes/neomorph.h \
    graphics/themes/sunset.h \
    graphics/themes/tealanalytics.h \
    graphics/themes/hudcyan.h \
    graphics/themes/light.h \
    logindialog.h \
    mainwindow.h \
    math/movingaverage.h \
    math/timing.h \
    math/tinyexpr.h \
    modules/abstractconfiguration.h \
    modules/abstractspecification.h \
    modules/arbgenerator/arbgenerator.h \
    modules/arbgenerator/arbgeneratorconfig.h \
    modules/arbgenerator/arbgeneratordefs.h \
    modules/arbgenerator/arbgeneratorfileloader.h \
    modules/arbgenerator/arbgeneratorspec.h \
    modules/arbgenerator/arbgeneratorwindow.h \
    modules/arbgenerator/arbgenpanelsettings.h \
    modules/arbgenerator/arbgensweepcontroller.h \
    modules/arbgenerator/gencommons.h \
    modules/arbgenerator/signalcreator.h \
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
    modules/numberparser.h \
    modules/patterngenerator/patterngenerator.h \
    modules/patterngenerator/patterngeneratorconfig.h \
    modules/patterngenerator/patterngeneratordefs.h \
    modules/patterngenerator/patterngeneratorpainter.h \
    modules/patterngenerator/patterngeneratorpatterns.h \
    modules/patterngenerator/patterngeneratorsettings.h \
    modules/patterngenerator/patterngeneratorspec.h \
    modules/patterngenerator/patterngeneratorwindow.h \
    modules/pwmgenerator/pwmgenerator.h \
    modules/pwmgenerator/pwmgeneratorconfig.h \
    modules/pwmgenerator/pwmgeneratorspec.h \
    modules/pwmgenerator/pwmgeneratorwindow.h \
    modules/scope/fftengine.h \
    modules/scope/mathcalculations.h \
    modules/scope/meascalculations.h \
    modules/scope/measurement.h \
    modules/scope/paneladvanced.h \
    modules/scope/panelcursors.h \
    modules/scope/panelmath.h \
    modules/scope/panelmeasurement.h \
    modules/scope/panelsettings.h \
    modules/scope/scope.h \
    modules/scope/scopeconfig.h \
    modules/scope/scopedefs.h \
    modules/scope/scopespec.h \
    modules/scope/scopewindow.h \
    modules/scope/timebaseandmemory.h \
    modules/syncpwm/syncpwm.h \
    modules/syncpwm/syncpwmconfig.h \
    modules/syncpwm/syncpwmdefs.h \
    modules/syncpwm/syncpwmpainter.h \
    modules/syncpwm/syncpwmsettings.h \
    modules/syncpwm/syncpwmspec.h \
    modules/syncpwm/syncpwmwindow.h \
    modules/template/templatemodule.h \
    modules/template/templatemoduleconfig.h \
    modules/template/templatemodulespec.h \
    modules/template/templatemodulewindow.h \
    modules/voltagesource/voltagesource.h \
    modules/voltagesource/voltagesourceconfig.h \
    modules/voltagesource/voltagesourcedefs.h \
    modules/voltagesource/voltagesourcespec.h \
    modules/voltagesource/voltagesourcewindow.h \
    modules/voltmeter/channeldata.h \
    modules/voltmeter/voltmeter.h \
    modules/voltmeter/voltmeterconfig.h \
    modules/voltmeter/voltmeterdefs.h \
    modules/voltmeter/voltmeterspec.h \
    modules/voltmeter/voltmeterwindow.h \
    settingsdialog.h



FORMS += \
    GUI/widgetbuttons.ui \
    GUI/widgetchart.ui \
    GUI/widgetcustomplot.ui \
    GUI/widgetdesciptionexpand.ui \
    GUI/widgetdial.ui \
    GUI/widgetdialrange.ui \
    GUI/widgetdisplay.ui \
    GUI/widgetfooter.ui \
    GUI/widgetlabel.ui \
    GUI/widgetlabelarea.ui \
    GUI/widgetlist.ui \
    GUI/widgetlogininfo.ui \
    GUI/widgetmodule.ui \
    GUI/widgetselection.ui \
    GUI/widgetseparator.ui \
    GUI/widgetswitch.ui \
    GUI/widgettab.ui \
    GUI/widgettextinput.ui \
    logindialog.ui \
    mainwindow.ui \
    modules/arbgenerator/arbgeneratorwindow.ui \
    modules/counter/counterwindow.ui \
    modules/device/devicewindow.ui \
    modules/patterngenerator/patterngeneratorwindow.ui \
    modules/pwmgenerator/pwmgeneratorwindow.ui \
    modules/scope/scopewindow.ui \
    modules/syncpwm/syncpwmwindow.ui \
    modules/template/templatemodulewindow.ui \
    modules/voltagesource/voltagesourcewindow.ui \
    modules/voltmeter/voltmeterwindow.ui \
    settingsdialog.ui


TRANSLATIONS += \
    LEO_sniffy_en_GB.ts

QMAKE_CXXFLAGS += \
    -Wall -Wextra -Wpedantic

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    graphics.qrc
