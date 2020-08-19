#ifndef COUNTERWINDOW_H
#define COUNTERWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QDebug>
#include <QTimer>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgettab.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetdisplay.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

#include "counterspec.h"
#include "counterconfig.h"
#include "counterdefs.h"
#include "countertabhighfreq.h"
#include "countertablowfreq.h"

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
    WidgetDisplay *displayRef;
    WidgetDisplay *displayInt;

    widgetTab *tabs;
    CounterTabHighFreq *tabHighFreq;
    CounterTabLowFreq *tabLowFreq;

    void setSpecification(CounterSpec *spec);
    void displayFlagHoldOn(WidgetDisplay *display, bool visible);
    void displayFlagSwitchMode(WidgetDisplay *display, bool visible);
    void showPMErrorSigns(WidgetDisplay *display, bool visible);
    void clearDisplay(WidgetDisplay *display);
    void msleep(int msec);

    void hfSetColorRemainSec(QColor color);

private:
    Ui::CounterWindow *ui;
    CounterConfig *conf;
    CounterSpec *spec;
    int modePrevIndex;

    void createCounterTabs(void);
    void configureCounterTabs(void);
    void createAllDisplays(void);
    WidgetDisplay* createLFDisplays(void);
    void configureAllDisplays(void);
    void configureDisplaysStaticAttr(WidgetDisplay *display, QString channel, QString sideLabelsColor);
    void configureDisplaysDynamicAttr();
    void configureHFLFErrorStyles(WidgetDisplay *display);
    void resetPreviousCounterMode();
    void setNextCounterMode(int index);
    void switchQuantity(int index, WidgetDisplay *display);    

    void lfEnableTabsComponents(bool enable);
    void lfSetDutyCycle(WidgetDisplay *display, WidgetDisplay *norDisplay, QString pin);
    void lfResetDutyCycle(WidgetDisplay *display, WidgetDisplay *norDisplay);
    void lfShowDutyCycleDisplays(WidgetDisplay *display, bool dutyEnable);

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
