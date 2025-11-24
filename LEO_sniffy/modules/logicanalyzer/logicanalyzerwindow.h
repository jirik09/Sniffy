#ifndef LOGICANALYSERWINDOW_H
#define LOGICANALYSERWINDOW_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "../../GUI/widgetchart.h"
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
    void triggerEdgeChanged(int edge);       // 0: Rising, 1: Falling
    void streamModeChanged(bool continuous); // true = continuous, false = batch

private:
    Ui::LogicAnalyzerWindow *ui;
    widgetChart *chart; // actually PatternChart for channel labeling
    LogicAnalyzerConfig *conf;
    QList<QString> specChannelNames; // cached for potential future UI use
    qint64 totalSamples = 0;         // streaming sample counter
    double activeSampleRate = 0.0;   // current stream rate
    bool continuousMode = false;
    bool autoScrollEnabled = true;  // when true keep view on most recent data
    bool paused = false;            // when true ignore incoming data
    int decimationFactor = 1;       // show only every Nth sample (>=1)
    double viewWindowSeconds = 0.0; // 0 => full history, otherwise scroll window size
};

#endif // LOGICANALYSERWINDOW_H
