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

    WidgetDisplay *displayCh1;
    WidgetDisplay *displayCh2;

    widgetTab *tabs;
    CounterTabHighFreq *tabHighFreq;

    void setColorHoldOn(QColor color);
    void specReceived(CounterSpec *spec);
    void displayHoldOnFlag(bool visible);

private:
    Ui::CounterWindow *ui;    

    void createCounterTabs(void);
    void configureCounterTabs(void);
    void createTwoDisplays(void);
    WidgetDisplay* createDisplay(void);
    void configureDisplays(void);

signals:

private slots:
    void switchCounterModeCallback(int index);
    void switchQuantityCallback(int index);
    void switchErrorAvgCallback(int index);
};

#endif // COUNTERWINDOW_H
