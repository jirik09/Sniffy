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
#include "channeldata.h"
#include "./graphics/colors.h"
#include "../labelformator.h"

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
    void showData(ChannelData data[], int numChannels);
    void showEmptyCalcs();
    void showVddValue(qreal value);
    void showProgress(int current, int max);
    void setPins(QString pins[], int numOfCh);

private:
    Ui::VoltmeterWindow *ui;
    VoltmeterConfig *config;

    QList<WidgetDisplay *> displays;

    widgetTab *tabs;
    WidgetButtons *buttonsChannelEnable;
    WidgetLabel *labelVdd;
    WidgetDialRange *dialAveraging;
    WidgetLabel *labelProgress;
    WidgetButtons *buttonsChannelSettings;

    WidgetButtons *voltageACDC;
    WidgetButtons *buttonsCalc;



private slots:
    void channelEnableCallback(int status);
    void averagingCallback(int value);
    void channelSettingsCallback(int clicked);
    void voltageCallback(int clicked);
    void buttonsCalcsCallback(int clicked);

signals:
    void averagingChanged(int value);
    void numChannelsEnabledChanged(int numChEnabled);
    void resetMinMax();
};

#endif // VOLTMETERWINDOW_H
