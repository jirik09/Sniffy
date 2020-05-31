#ifndef WINDOWSCOUNTER_H
#define WINDOWSCOUNTER_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include "widgetfeature.h"
#include "widgettop.h"
#include "widgetseparator.h"
#include "widgetdial.h"
#include "widgetdialrange.h"
#include "widgetswitch.h"
#include "widgetbuttons.h"
#include "widgetlabel.h"
#include "widgetselection.h"

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
