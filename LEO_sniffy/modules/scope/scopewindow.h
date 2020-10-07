#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QtCore/QtMath>
#include <QDebug>

#include "../../GUI/widgetcontrolmodule.h"
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

#include "../labelformator.h"

#include "scopedefs.h"
#include "panelsettings.h"
#include "panelmeasurement.h"
#include "panelcursors.h"
#include "scopeconfig.h"

#define CHART_MAX_Y 7
#define CHART_MIN_Y -1

namespace Ui {
class ScopeWindow;
}

class ScopeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScopeWindow(QWidget *parent = nullptr);
    ~ScopeWindow();
    void paintEvent(QPaintEvent *event);


    void showDataTraces(QVector<QVector<QPointF>> dataSeries, float timeBase, int triggerChannelIndex);
    void paintTraces(QVector<QVector<QPointF>> dataSeries);
    void setDataMinMaxTimeAndZoom(qreal minX, qreal maxX, qreal zoom);

    void singleSamplingDone();
    void samplingOngoing();
    void triggerCaptured();
    void setRealSamplingRate(int smpl);

    void updateMeasurement(QList<Measurement*> m);
    void passConfig(ScopeConfig &conf);


signals:
    void timeBaseChanged(float value);
    void memoryLengthChanged(int length);
    void triggerModeChanged(ScopeTriggerMode);
    void triggerEdgeChanged(ScopeTriggerEdge);
    void pretriggerChanged(float value);
    void triggerValueChanged(float value);
    void triggerChannelChanged(int index);
    void channelEnableChanged(int buttonStatus);
    void measurementChanged(Measurement *m);
    void measurementClearChanged();
    void verticalChanged();


private slots:
    void channelVerticalCallback(int index);
    void channelVerticalScaleCallback(float value);
    void channelVerticalShiftCallback(float value);
    void timeBaseCallback(float value);
    void channelEnableCallback(int buttonStatus);
    void longMemoryCallback(int index);
    void pretriggerCallback(float value);
    void triggerValueCallback(float value);
    void triggerChannelCallback(int index);
    void triggerEdgeCallback(int index);
    void triggerModeCallback(int index);
    void measurementAddedCallback(Measurement *m);
    void measurementClearCallback();
    void sliderShiftCallback(int value);

    void chartLocalZoomCallback();


public:
    Ui::ScopeWindow *ui;
    PanelSettings *panelSet;
private:
    ScopeConfig *config;
    widgetChart *chart;

    QVector<QVector<QPointF>> ChartData;
    int triggerChannelIndex =0;

    WidgetLabelArea *labelInfoPanel;

    PanelMeasurement *panelMeas;

    PanelCursors *panelCursors;

    void updateChartTimeScale(float timeBase);
    void fillTimeBase();

    float previousTimeBase = 0;
};

#endif // WINDOWSCOPE_H
