#ifndef COUNTERWINDOW_H
#define COUNTERWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QDebug>

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

    WidgetDisplay *displayCh1;
    WidgetDisplay *displayCh2;

    widgetTab *tabs;
    CounterTabHighFreq *tabHighFreq;
    CounterTabLowFreq *tabLowFreq;

    void specReceived(CounterSpec *spec);
    void displayFlagHoldOn(WidgetDisplay *display, bool visible);
    void displayFlagSwitchMode(WidgetDisplay *display, bool visible);
    void showPMErrorSigns(WidgetDisplay *display, bool visible);
    void reconfigDisplayLabelArea(CounterSpec *spec);

    void hfSetColorRemainSec(QColor color);

private:
    Ui::CounterWindow *ui;
    CounterConfig *conf;
    CounterSpec *spec;

    void createCounterTabs(void);
    void configureCounterTabs(void);
    void createTwoDisplays(void);
    WidgetDisplay* createDisplay(void);
    void configureDisplays(void);
    void switchQuantity(int index, WidgetDisplay *display);

    void lfDisableButtons();
    void lfEnableButtons();

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
