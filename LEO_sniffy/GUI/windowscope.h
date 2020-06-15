#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QtCore/QtMath>
#include "widgetfeature.h"
#include "widgettop.h"
#include "widgetseparator.h"
#include "widgetdial.h"
#include "widgetdialrange.h"
#include "widgetswitch.h"
#include "widgetbuttons.h"
#include "widgetlabel.h"
#include "widgetselection.h"
#include "widgetchart.h"
#include "widgettab.h"

#include "../features/scope/scope.h"


namespace Ui {
class WindowScope;
}

class WindowScope : public QWidget
{
    Q_OBJECT

public:
    explicit WindowScope(QWidget *parent = nullptr);
    ~WindowScope();

    void setScope(Scope * scp);
    void setModuleWidget(WidgetFeature *scopeLeftWidget);
    void windowOpen();

signals:

    void startScope();
    void stopScope();


private slots:
    void generalSettingsClicked();
    void generalMeasClicked();

    void enableModuleWidget();
    void visibilityChanged(bool vis);

    void dataReceived(QVector<QVector<QPointF>> dataSeries);

private:
    Ui::WindowScope *ui;

    void fillTimeBase();

    WindowScope *scopeWindow;
    WidgetFeature *scopeModuleWidget;

    WidgetTop *WidgetTopSelection;


    QWidget *WidgetGeneral;
    WidgetDial *dialTimeBase;
    QScrollArea *scrollAreaGeneral;
    QVBoxLayout *verticalLayoutGeneral;


    QWidget *WidgetMeas;
    QScrollArea *scrollAreaMeas;
    QVBoxLayout *verticalLayoutMeas;

    Scope *scope;

    widgetChart *chart;


};

#endif // WINDOWSCOPE_H
