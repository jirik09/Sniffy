#ifndef WIDGETCHART_H
#define WIDGETCHART_H

#include <QWidget>
#include <QList>
#include <QtCharts/QXYSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class widgetChart;
}

class widgetChart : public QWidget
{
    Q_OBJECT

public:
    explicit widgetChart(QWidget *parent = nullptr, int maxTraces = 1);
    ~widgetChart();
    void clearAll();
    void updateTrace(QVector<QPointF> *points, int index);
    void repaint();


private:
    Ui::widgetChart *ui;
    QList<QXYSeries *> seriesList;
    int maxTraces;
};

#endif // WIDGETCHART_H
