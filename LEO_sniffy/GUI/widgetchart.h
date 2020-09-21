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
#include <QMenu>
#include <QtMath>
#include <QDateTimeAxis>

#include "../graphics/colors.h"

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
    void clearPoints(int startIndex, int endIndex);
    void clearPoint(int traceIndex, int index);
    void updateTrace(QVector<QPointF> *points, int index);
    void appendToTrace(int index, QVector<QPointF> *points);
    void updateAxis();
    void setMaxX(float max);
    void setDataMinMax(qreal minX, qreal maxX);
    void setRangeX(qreal minX, qreal maxX);
    void setRangeY(qreal minY, qreal maxY);
    void setRange(qreal minX, qreal maxX, qreal minY, qreal maxY);
    void setMargins(int left, int top, int right, int bottom);

    void setZoom(float invZoom);
    qreal getZoom();
    void setShift (float shift);

    void setGridLinesVisible(bool gridVisibleX, bool gridVisibleY);
    void setGridDensity(int tickX, int tickY);

    void formatAxisLabelsForScope();
    void formatLabels(QString axisXLabelForm, QString axisYLabelForm);
    void setGraphColor(QColor qColor);
    void setLabelsVisible(bool lableVisibleX, bool lableVisibleY);
    void setLabelsSize(int pointSize);
    void createHorizontalMarkes();
    void setHorizontalMarker(int channelIndex, qreal value);

private:
    Ui::widgetChart *ui;
    QList<QXYSeries *> seriesList;
    QScatterSeries *markersHorizontal;
    enum markers {ENABLED, DISABLED} markers = DISABLED;
    int maxTraces;

    qreal minX = 0;
    qreal maxX = 1;

    qreal invZoom = 1;
    qreal shift = 0.5;

    QChart *chart;

    QValueAxis *axisX;   // QAbstractAxis
    QValueAxis *axisY;
    QValueAxis *axisMarkerHorizontal;

    QColor colors[4] = {QCOLOR_ORANGE, QCOLOR_BLUE,
                        QCOLOR_GREEN, QCOLOR_PURPLE};

signals:
    void chartRightClicked(const QPoint &pos);

private slots:
    void chartRightClickCallback(const QPoint &mousePos);

};

#endif // WIDGETCHART_H
