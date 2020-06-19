#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QtCore/QtMath>
#include <QDebug>

#include "../../GUI/widgetcontrolmodule.h"
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

#include "scopedefs.h"


namespace Ui {
class WindowScope;
}

class WindowScope : public QWidget
{
    Q_OBJECT

public:
    explicit WindowScope(QWidget *parent = nullptr);
    ~WindowScope();

    void dataReceived(QVector<QVector<QPointF>> dataSeries, float timeBase);


signals:
    void timeBaseChanged(float value);
    void triggerChanged(ScopeTriggerMode);
    void pretriggerChanged(float value);

private slots:
    void timeBaseCallback(float value);
    void channelEnableCallback(int buttonStatus);
    void pretriggerCallback(float value);
    void triggerValueCallback(float value);
    void triggerChannelCallback(int index);
    void triggerModeCallback(int index);

private:
    Ui::WindowScope *ui;

    void fillTimeBase();

    WidgetDial *dialTimeBase;
    WidgetButtons *buttonsChannelEnable;
    WidgetButtons *buttonsTriggerMode;
    WidgetButtons *buttonsTriggerChannel;
    WidgetButtons *buttonsTriggerEdge;

    WidgetDialRange *dialPretrigger;
    WidgetDialRange *dialTriggerValue;

    widgetChart *chart;
};

#endif // WINDOWSCOPE_H
