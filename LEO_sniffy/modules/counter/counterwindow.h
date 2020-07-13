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

namespace Ui {
class CounterWindow;
}

class CounterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CounterWindow(QWidget *parent = nullptr);
    ~CounterWindow();

    WidgetDisplay* hfDisplay(void);
    WidgetDisplay* lfDisplay1(void);
    WidgetDisplay* lfDisplay2(void);

private:
    Ui::CounterWindow *ui;
    widgetTab *tabs;
    WidgetDisplay *HFDisplay;
    WidgetDisplay *LFDisplay1;
    WidgetDisplay *LFDisplay2;

    void createCounterTabs(void);
    void createHighFrequencyDisplay(void);
    void createLowFrequencyDisplay(void);
    WidgetDisplay* addDisplayWidgetToTab(int tabNumber);
};

#endif // COUNTERWINDOW_H
