#ifndef WINDOWSCOUNTER_H
#define WINDOWSCOUNTER_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include "../../GUI/widgetfeature.h"
#include "../../GUI/widgettop.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"

namespace Ui {
class WindowCounter;
}

class WindowCounter : public QWidget
{
    Q_OBJECT

public:
    explicit WindowCounter(QWidget *parent = nullptr);
    ~WindowCounter();


private:
    Ui::WindowCounter *ui;

    WindowCounter *counterWindow;
    WidgetFeature *counterLeftWidget;

    WidgetTop *WidgetTopSelection;

    QWidget *WidgetHighFrequency;
    QScrollArea *scrollAreaHighFrequency;
    QVBoxLayout *verticalLayoutHighFrequency;

    QWidget *WidgetLowFrequency;
    QScrollArea *scrollAreaLowFrequency;
    QVBoxLayout *verticalLayoutLowFrequency;


private slots:
    void highFrequencyClicked();
    void lowFrequencyClicked();
    void frequencyRatioClicked();
    void timeIntervalClicked();

};

#endif // WINDOWSCOUNTER_H
