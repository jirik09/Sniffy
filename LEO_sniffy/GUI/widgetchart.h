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
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneMouseEvent>

#include <QMenu>
#include <QtMath>
#include <QDateTimeAxis>
#include <QDebug>

#include "../graphics/colors.h"
#include "../graphics/styles.h"
#include "../math/timing.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class widgetChart;
}

enum class MarkerType
{
ARROW_DOWN_BIG, ARROW_UP_SMALL, ARROW_DOWN_SMALL, TICK, CROSS, CIRCLE
};

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

    void setDataMinMax(qreal minX, qreal maxX);
    void setRangeX(qreal minX, qreal maxX);
    void setRangeY(qreal minY, qreal maxY);
    void setRange(qreal minX, qreal maxX, qreal minY, qreal maxY);
    void setMargins(int left, int top, int right, int bottom);

    void setZoom(float invZoom);
    qreal getZoom();
    qreal getLocalZoom();
    void setShift (float shift);
    qreal getShift();

    void setGridLinesVisible(bool gridVisibleX, bool gridVisibleY);
    void setGridDensity(int tickX, int tickY);

    void formatAxisLabelsForScope();
    void formatLabels(QString axisXLabelForm, QString axisYLabelForm);
    void setGraphColor(QColor qColor);
    void setLabelsVisible(bool lableVisibleX, bool lableVisibleY);
    void setLabelsSize(int pointSize);
    void setHorizontalMarker(int channelIndex, qreal value, MarkerType type = MarkerType::TICK);
    void setVerticalMarker(int channelIndex, qreal value);

private:
    Ui::widgetChart *ui;
    QList<QXYSeries *> seriesList;
    QList<QScatterSeries *> markersHorizontal;
    int markerHorizontalIndex=0;
    QList<QScatterSeries *> markersVertical;
    int markerVerticalIndex=0;

    QMenu *menu;
    QAction *spline, *line, *scatter, *btnOpenGL;
    enum enable {DISABLED, ENABLED} openGL = DISABLED;

    int maxTraces;

    qreal minX = 0;
    qreal maxX = 1;

    qreal invZoom = 1;
    qreal shift = 0.5;
    qreal localZoom = 1;
    bool mousePressed = false;
    QPointF mousePressedPoint;
    qreal initMouseShift;

    QChart *chart;

    QValueAxis *axisX;   // QAbstractAxis
    QValueAxis *axisY;
    QValueAxis *axisX_MarkerHorizontal;
    QValueAxis *axisY_MarkerVertical;
    QValueAxis *axisX_FFT;

    QPainterPath *MarkerPath_ArrowDownBig;
    QPainterPath *MarkerPath_ArrowDownSmall;
    QPainterPath *MarkerPath_ArrowUpSmall;
    QPainterPath *MarkerPath_Tick;
    QPainterPath *MarkerPath_Cross;
    QPainterPath *MarkerPath_Circle;

    void initContextMenu();
    void createHorizontalMarkers();
    void createVerticalMarkers();
    void initBrushes();
    QBrush getBrush (int channelIndex, MarkerType type);

    void createSeries(QAbstractSeries *series);

    bool eventFilter(QObject *obj, QEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

signals:
    void chartSeriesChanged();

private slots:
    void switchToSplineSeriesCallback();
    void switchToLineSeriesCallback();
    void switchToScatterSeriesCallback();
    void useOpenGLCallback();

    void rightClickCallback(const QPoint &mousePos);
    void hovered(const QPointF &point);
};

#endif // WIDGETCHART_H
