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
#include "../../GUI/widgetlabelarea.h"


#include "scopedefs.h"
#include "panelsettings.h"
#include "panelmeasurement.h"

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

    void singleSamplingDone();
    void samplingOngoing();
    void triggerCaptured();
    void setRealSamplingRate(int smpl);


signals:
    void timeBaseChanged(float value);
    void triggerModeChanged(ScopeTriggerMode);
    void triggerEdgeChanged(ScopeTriggerEdge);
    void pretriggerChanged(float value);
    void triggerValueChanged(float value);
    void triggerChannelChanged(int index);
    void channelEnableChanged(int buttonStatus);


private slots:
    void timeBaseCallback(float value);
    void channelEnableCallback(int buttonStatus);
    void pretriggerCallback(float value);
    void triggerValueCallback(float value);
    void triggerChannelCallback(int index);
    void triggerEdgeCallback(int index);
    void triggerModeCallback(int index);

private:
    Ui::WindowScope *ui;


    widgetChart *chart;
    widgetLabelArea *labelInfoPanel;

    PanelSettings *panelSet;
    PanelMeasurement *panelMeas;

    void updateChartScale(float timeBase);
    void fillTimeBase();
};

#endif // WINDOWSCOPE_H
