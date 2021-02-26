#ifndef VOLTMETERWINDOW_H
#define VOLTMETERWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QDebug>
#include <QVBoxLayout>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgettab.h"
#include "../../GUI/widgetlabelarea.h"
#include "../../GUI/widgetdisplay.h"
#include "../../GUI/widgetswitch.h"

#include "voltmeterconfig.h"
#include "voltmeterdefs.h"

namespace Ui {
class VoltmeterWindow;
}

class VoltmeterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VoltmeterWindow(VoltmeterConfig *config, QWidget *parent = nullptr);
    ~VoltmeterWindow();

    void restoreGUIAfterStartup();

private:
    Ui::VoltmeterWindow *ui;
    VoltmeterConfig *config;

    QList<WidgetDisplay *> displays;

    widgetTab *tabs;
    WidgetButtons *buttonsChannelEnable;
    WidgetDialRange *dialAveraging;
    WidgetButtons *buttonsChannelSettings;

    WidgetButtons *voltageACDC;
    WidgetButtons *buttonsClac;

private slots:
    void channelEnableCallback(int status);
    void averagingCallback(int value);
    void channelSettingsCallback(int clicked);
    void voltageCallback(int clicked);
    void buttonsCalsCallback(int clicked);


};

#endif // VOLTMETERWINDOW_H
