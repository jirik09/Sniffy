#ifndef WIDGETCUSTOMPLOT_H
#define WIDGETCUSTOMPLOT_H

#include <QWidget>
#include <QDebug>

#include "../graphics/graphics.h"
#include "../GUI/qcustomplot.h"

namespace Ui {
class WidgetCustomPlot;
}

class WidgetCustomPlot : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetCustomPlot(QWidget *parent = nullptr, int traceCapacity = 1);
    ~WidgetCustomPlot();

    void clearAll();
    void updateTrace(QVector<QPointF> *points, int index);

    void setRangeX(qreal minX, qreal maxX);
    void setRangeY(qreal minY, qreal maxY);
    void setRange(qreal minX, qreal maxX, qreal minY, qreal maxY);

    void setTraceColor(int index, QColor color);
    void setGridLinesVisible(bool gridVisibleX, bool gridVisibleY);
    void setGridDensity(int tickX, int tickY);
    void setGridHorizontalDensity(int tickX);

    void setGraphColor(QColor qColor);

    void setLabelsVisible(bool labelVisibleX, bool labelVisibleY);

private:
    Ui::WidgetCustomPlot *ui;

    QCustomPlot *plot;
    QCPGraph *graph;
    QCPAxisRect *axis;
    //QCPAxis *xAxis, *yAxis;
    QSharedPointer<QCPAxisTickerTime> ticker;

    int maxTraces;        

signals:

};

#endif // WIDGETCUSTOMPLOT_H
