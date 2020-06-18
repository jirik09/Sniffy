#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QtCore/QtMath>

#include "../../GUI/widgetfeature.h"
#include "../../GUI/widgettop.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgettab.h"

#include "scope.h"


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
    void enableModuleWidget();
    void visibilityChanged(bool vis);

    void dataReceived(QVector<QVector<QPointF>> dataSeries);

    void timeBaseCallback(float value);
    void channelEnableCallback(int buttonStatus);
    void triggerValueCallback(float value);
    void triggerChannelCallback(int index);
    void triggerModeCallback(int index);

private:
    Ui::WindowScope *ui;

    void fillTimeBase();

    WidgetFeature *scopeModuleWidget;




    WidgetDial *dialTimeBase;
    WidgetButtons *buttonsChannelEnable;
    WidgetButtons *buttonsTriggerMode;
    WidgetButtons *buttonsTriggerChannel;
    WidgetButtons *buttonsTriggerEdge;

    WidgetDialRange *dialPretrigger;
    WidgetDialRange *dialTriggerValue;





    Scope *scope;

    widgetChart *chart;


};

#endif // WINDOWSCOPE_H
