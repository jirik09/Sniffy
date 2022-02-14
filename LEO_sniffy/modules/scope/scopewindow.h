#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>
#include <QtCore/QtMath>
#include <QDebug>
#include <QSplitter>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
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
#include "panelmath.h"
#include "paneladvanced.h"
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
    explicit ScopeWindow(ScopeConfig *config, QWidget *parent = nullptr);
    ~ScopeWindow();
    void paintEvent(QPaintEvent *event);


    void showDataTraces(QVector<QVector<QPointF>> dataSeries, float timeBase, int triggerChannelIndex);

    void setDataMinMaxTimeAndZoom(qreal minX, qreal maxX, qreal zoom);

    void singleSamplingDone();
    void samplingOngoing();
    void triggerCaptured();
    void setRealSamplingRateAndLlength(int smpl, int len);
    void setNumChannels(int channels);

    void updateMeasurement(QList<Measurement*> m);
    void updateMath(QVector<QPointF> mathTrace);
    void updateFFTchart(QVector<QPointF> fftTrace, qreal maxFreq);
    void mathError(int errorPosition);
    void restoreGUIAfterStartup();


signals:
    void timeBaseChanged(float value);
    void memoryPolicyChanged(int index);
    void memoryCustomLengthChanged(int length);
    void samlingFrequecyCustomInputChanged(int freq);
    void triggerModeChanged(ScopeTriggerMode);
    void triggerEdgeChanged(ScopeTriggerEdge);
    void pretriggerChanged(float value);
    void triggerValueChanged(float value);
    void triggerChannelChanged(int index);
    void channelEnableChanged(int buttonStatus);
    void measurementChanged(Measurement *m);
    void mathExpressionChanged(QString exp);
    void fftChanged(int length, FFTWindow window, FFTType type, int channelIndex);
    void measurementClearChanged();
    void verticalChanged();
    void resolutionChanged(int resolution);


private slots:
    void channelVerticalCallback(int index);
    void channelVerticalScaleCallback(float value);
    void channelVerticalShiftCallback(float value);
    void timeBaseCallback(float value);
    void SamplingFreqCustomInputCallback (int freq);
    void dataLengthInputCallback (int length);
    void channelEnableCallback(int buttonStatus);
    void memoryPolicyCallback(int index);
    void memoryCustomLengthCallback(int index);
    void pretriggerCallback(float value);
    void triggerValueCallback(float value);
    void triggerChannelCallback(int index);
    void triggerEdgeCallback(int index);
    void triggerModeCallback(int index);
    void measurementAddedCallback(Measurement *m);
    void measurementClearCallback();
    void mathExpressionCallback(QString exp);
    void fftChangedCallback(int length, FFTWindow window, FFTType type, int channelIndex);
    void fftchartChangedCallback(qreal scale, qreal shift, bool isLog = false);
    void mathTypeChangedCallback(int index);
    void sliderShiftCallback(int value);

    void chartLocalZoomCallback();

    void cursorTypeCallback(int index);
    void cursorChannelCallback(int index);
    void cursorValueHorACallback(float value);
    void cursorValueHorBCallback(float value);
    void cursorValueVerACallback(float value);
    void cursorValueVerBCallback(float value);
    void resolutionChangedCallback(int index);

    void updateCursorReadings();
    void setHorizontalCursor(int channelIndex, qreal value, Cursor type);


public:

private:
    Ui::ScopeWindow *ui;
    PanelSettings *panelSet;
    ScopeConfig *config;
    widgetChart *chart;

    widgetChart *chartFFT;
    QSplitter *splitter;

    QVector<QVector<QPointF>> ChartData;
    QVector<QPointF> ChartMathData;
    QVector<QPointF> ChartFFTData;
    int fftChannelIndex = 0;
    int triggerChannelIndex = 0;

    WidgetLabelArea *labelInfoPanel;
    PanelMeasurement *panelMeas;
    PanelCursors *panelCursors;
    PanelMath *panelMath;
    PanelAdvanced *panelAdvanced;

    void updateChartTimeScale(float timeBase);
    void fillTimeBase();
    void paintTraces(QVector<QVector<QPointF>> dataSeries, QVector<QPointF> mathSeries);
    void paintMath(QVector<QPointF> mathSeries);
};

#endif // WINDOWSCOPE_H
