#ifndef LOGICANALYSERWINDOW_H
#define LOGICANALYSERWINDOW_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgetbuttons.h"
#include "../patterngenerator/patternchart.h"
#include "logicanalyzerconfig.h"

namespace Ui
{
    class LogicAnalyzerWindow;
}

class LogicAnalyzerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogicAnalyzerWindow(LogicAnalyzerConfig *conf, QWidget *parent = nullptr);
    ~LogicAnalyzerWindow();

    void showData(const QVector<QVector<int>> &data, double sampleRate);
    void setSpecification(const QList<QString> &channelNames);
    void resetStream();

signals:
    void startCapture();
    void stopCapture();
    void sampleRateChanged(int rate);
    void triggerChanged(int channel, int type, int level);
    void triggerChannelChanged(int channel);
    void triggerEdgeChanged(int edge); // 0: Rising, 1: Falling
    void triggerModeChanged(int mode); // 0: Auto, 1: Normal, 2: Single (Mapped to ScopeTriggerMode-like enums)

private:
    Ui::LogicAnalyzerWindow *ui;
    widgetChart *chart; // actually PatternChart for channel labeling
    LogicAnalyzerConfig *conf;
    WidgetButtons *buttonsTriggerMode; // Replaces Start/Stop/Mode
    QList<QString> specChannelNames;   // cached for potential future UI use
    qint64 totalSamples = 0;           // streaming sample counter
    double activeSampleRate = 0.0;     // current stream rate
    bool continuousMode = false;
    bool paused = false;      // when true ignore incoming data
    int decimationFactor = 1; // show only every Nth sample (>=1)

    void triggerModeCallback(int index);
};

#endif // LOGICANALYSERWINDOW_H
