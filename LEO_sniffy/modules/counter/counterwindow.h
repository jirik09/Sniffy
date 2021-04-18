#ifndef COUNTERWINDOW_H
#define COUNTERWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QDebug>
#include <QString>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgettab.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetdisplay.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../graphics/graphics.h"

#include "counterspec.h"
#include "counterconfig.h"
#include "counterdefs.h"
#include "countertabhighfreq.h"
#include "countertablowfreq.h"
#include "countertabratio.h"
#include "countertabintervals.h"

namespace Ui {
class CounterWindow;
}

class CounterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CounterWindow(CounterConfig *config, QWidget *parent = nullptr);
    ~CounterWindow();

    WidgetDisplay *displayHF;
    WidgetDisplay *displayLFCh1;
    WidgetDisplay *displayLFCh2;
    WidgetDisplay *displayRat;
    WidgetDisplay *displayInt;

    widgetTab *tabs;
    CounterTabHighFreq *tabHighFreq;
    CounterTabLowFreq *tabLowFreq;
    CounterTabRatio *tabRatio;
    CounterTabIntervals *tabInter;

    void setSpecification(CounterSpec *spec);
    void displayFlagAcquiring(WidgetDisplay *display, bool visible);
    void displayFlagSwitchMode(WidgetDisplay *display, bool visible);
    void ratDisplayFlagWarning(WidgetDisplay *display, bool visible);
    void intDisplayFlagTimeout(WidgetDisplay *display, bool visible);
    void showPMErrorSigns(WidgetDisplay *display, bool visible);    
    void clearDisplay(WidgetDisplay *display, bool uponSwitch);
    void setStartButton(bool start);

    void msleep(int msec);

    void hfSetColorRemainSec(bool waiting);

    void appendNewHistorySample(WidgetDisplay *display, QString prefix, float sample, QString affix = "", float timeStep = 1);
    void associateToHistorySample(WidgetDisplay *display, int traceIndex, QString prefix = "", float sample = 0, QString affix = "");

    void restoreGUIAfterStartup();

private:
    Ui::CounterWindow *ui;
    CounterConfig *conf;
    CounterSpec *spec;

    bool isAfterStart = true;

    void createCounterTabs(void);
    void configureCounterTabs(void);
    void createAllDisplays(void);
    WidgetDisplay* createLowFreqDisplays(QString name);
    WidgetDisplay* createRatioDisplay(void);
    WidgetDisplay* createHighFreqDisplay(void);
    WidgetDisplay* createIntervalsDisplay(void);
    void configureAllDisplays(void);
    void configureDisplaysStaticAttr(WidgetDisplay *display, QString channel, QString quantity, QString sideLabelsColor);
    void configureDisplaysDynamicAttr();
    void configureDisplaysDynamicLabels(WidgetDisplay *display, QString pin, int chan);
    void configureErrorStyles(WidgetDisplay *display);
    void resetPreviousCounterMode();
    void setNextCounterMode(int index);
    void switchQuantity(int index, WidgetDisplay *display);    

    void lfSetDutyCycle(WidgetDisplay *display, WidgetDisplay *norDisplay, QString pin);
    void lfResetDutyCycle(WidgetDisplay *display, WidgetDisplay *norDisplay);
    void lfShowDutyCycleDisplays(WidgetDisplay *display, bool dutyEnable);
    void lfShowErrorStyle(WidgetDisplay *display, bool show);

signals:

private slots:
    /* Common */
    void switchCounterModeCallback(int index);

    /* High Frequency mode */
    void hfSwitchQuantityCallback(int index);
    void hfSwitchErrorAvgCallback(int index);

    /* Low Frequency mode */
    void lfSwitchChannelCallback(int index);
    void lfSwitchQuantityCallback(int index);
    void lfSwitchDutyCycleCallback(int index);
};

#endif // COUNTERWINDOW_H
