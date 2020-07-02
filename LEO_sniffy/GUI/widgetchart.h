#ifndef WIDGETCHART_H
#define WIDGETCHART_H

#include <QWidget>
#include <QList>
#include <QtCharts/QXYSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QtMath>

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
    void updateAxis();
    void setMaxX(float max);
    void setDataMinMax(qreal minX, qreal maxX);

    void setZoom(float invZoom);
    qreal getZoom();
    void setShift (float shift);

    void setMarkerHorizontal(int channelIndex, qreal value);





private:
    Ui::widgetChart *ui;
    QList<QXYSeries *> seriesList;
    QScatterSeries *markersHorizontal;
    int maxTraces;

    qreal minX = 0;
    qreal maxX = 1;

    qreal invZoom = 1;
    qreal shift = 0.5;

    QChart *chart;

    QValueAxis *axisX;
    QValueAxis *axisY;
    QValueAxis *axisMarkerHorizontal;

    QColor colors[4];
};

#endif // WIDGETCHART_H
