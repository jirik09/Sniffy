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

#include "countertabhighfreq.h"

namespace Ui {
class CounterWindow;
}

class CounterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CounterWindow(QWidget *parent = nullptr);
    ~CounterWindow();

    WidgetDisplay* getDisplay1(void);
    WidgetDisplay* getDisplay2(void);

private:
    Ui::CounterWindow *ui;

    CounterTabHighFreq *tabHighFreq;

    widgetTab *tabs;
    WidgetDisplay *display1;
    WidgetDisplay *display2;

    void createCounterTabs(void);
    void createTwoDisplays(void);
    WidgetDisplay* addDisplayWidgetToMeasArea(void);
};

#endif // COUNTERWINDOW_H
