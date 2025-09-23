#ifndef WINDOWSCOPE_H
#define WINDOWSCOPE_H

#include <QWidget>
#include <QSplitter>
#include <limits>

#if defined(__cpp_lib_span) && __cpp_lib_span >= 202002L
    #include <span>
    template<typename T>
    using ViewSpan = std::span<T>;
#else
    template<typename T>
    class ViewSpan { // Minimal shim (read-only) if std::span not available
    public:
        ViewSpan(const T* data=nullptr, size_t size=0):m_data(data),m_size(size){}
        const T* begin() const { return m_data; }
        const T* end() const { return m_data + m_size; }
        const T* data() const { return m_data; }
        size_t size() const { return m_size; }
        bool empty() const { return m_size==0; }
    private:
        const T* m_data;
        size_t m_size;
    };
#endif

#include "scopedefs.h"
#include <limits>

// Lightweight FFT enum definitions (avoid including full fftengine in header)
#include "fftdefs.h"

// Forward declarations to keep header light
class widgetChart;
class WidgetLabelArea;
class PanelSettings;
class PanelMeasurement;
class PanelCursors;
class PanelMath;
class PanelAdvanced;
class ScopeConfig;
class Measurement;
class WidgetButtons;
class WidgetDial;
class WidgetDialRange;
class WidgetTextInput;

#include <QVector>
#include <QPointF>

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


    void showDataTraces(const QVector<QVector<QPointF>> &dataSeries, float timeBase, int triggerChannelIndex);

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
    void cursorValueHorCallback();
    void cursorValueVerCallback();
    void resolutionChangedCallback(int index);

    void updateCursorReadings();
    void setHorizontalCursors(int channelIndex);
    void setVerticalCursors(int channelIndex);


public:

private:
    struct ChannelTransformCache {
        float lastScale = std::numeric_limits<float>::quiet_NaN();
        float lastOffset = std::numeric_limits<float>::quiet_NaN();
        QVector<QPointF> transformed;
        bool dirty = true;
    };
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
    void paintTraces(const QVector<QVector<QPointF>> &dataSeries, const QVector<QPointF> &mathSeries);
    void paintMath(const QVector<QPointF> &mathSeries);
    void validateAndApplyTriggerChannel(int buttonStatus);
    void updateTriggerChannelButtons(int buttonStatus);
    // Transform cache helpers
    void initTransformCache();
    void invalidateAllTransforms();
    void invalidateChannelTransform(int channelIndex);
    const QVector<QPointF>& getTransformedChannel(int ch, const QVector<QPointF>& src) ;

    QVector<ChannelTransformCache> transformCache; // size TOTAL_SCOPE_TRACES
};

#endif // WINDOWSCOPE_H
