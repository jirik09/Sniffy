#ifndef LOGICANALYSERWINDOW_H
#define LOGICANALYSERWINDOW_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "../../GUI/widgetchart.h"
#include "logicanalyzerconfig.h"

namespace Ui {
class LogicAnalyzerWindow;
}

class LogicAnalyzerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogicAnalyzerWindow(LogicAnalyzerConfig *conf, QWidget *parent = nullptr);
    ~LogicAnalyzerWindow();

    void showData(const QVector<QVector<int>> &data, double sampleRate);

signals:
    void startCapture();
    void stopCapture();
    void sampleRateChanged(int rate);
    void triggerChanged(int channel, int type, int level);
    void triggerChannelChanged(int channel);
    void triggerEdgeChanged(int edge); // 0: Rising, 1: Falling

private:
    Ui::LogicAnalyzerWindow *ui;
    widgetChart *chart;
    LogicAnalyzerConfig *conf;
};

#endif // LOGICANALYSERWINDOW_H
