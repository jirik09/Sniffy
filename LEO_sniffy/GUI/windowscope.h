#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

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
class WindowScope;
}

class WindowScope : public QWidget
{
    Q_OBJECT

public:
    explicit WindowScope(QWidget *parent = nullptr);
    ~WindowScope();


private:
    Ui::WindowScope *ui;

    void fillTimeBase();

    WindowScope *scopeWindow;
    WidgetFeature *scopeLeftWidget;

    WidgetTop *WidgetTopSelection;


    QWidget *WidgetGeneral;
    WidgetDial *dialTimeBase;
    QScrollArea *scrollAreaGeneral;
    QVBoxLayout *verticalLayoutGeneral;


    QWidget *WidgetMeas;
    QScrollArea *scrollAreaMeas;
    QVBoxLayout *verticalLayoutMeas;


private slots:
    void generalSettingsClicked();
    void generalMeasClicked();

};

#endif // WINDOWSCOPE_H
