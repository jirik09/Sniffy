#include "widgetcustomplot.h"
#include "ui_widgetcustomplot.h"

WidgetCustomPlot::WidgetCustomPlot(QWidget *parent, int maxTraces) :
    QWidget(parent),
    ui(new Ui::WidgetCustomPlot),
    maxTraces(maxTraces)
{
    ui->setupUi(this);
    setStyleSheet("background-color:" + Graphics::COLOR_DATA_INPUT_AREA);

    plot = new QCustomPlot(parent);
    plot->setObjectName("plot");

    plot->setBackground(QColor(Graphics::COLOR_CHART));
    plot->setMouseTracking(true);
    plot->setInteractions(QCP::iNone);

    plot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    plot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    //plot->yAxis->ticker()->setTickCount(7);

    plot->yAxis->setTicks(false);
    plot->xAxis->setSubTicks(false);
    plot->yAxis->setSubTicks(false);

    for(int i = 0; i < maxTraces; i++){
        plot->addGraph();
        plot->graph(i)->setPen(QPen(QBrush(QColor(Graphics::getChannelColor(i))), 2));
        plot->graph(i)->setLineStyle(QCPGraph::lsLine);
        plot->graph(i)->setAntialiased(false);
    }            

    ui->horizontalLayout_plot->addWidget(plot);
}

WidgetCustomPlot::~WidgetCustomPlot()
{
    delete ui;
}

void WidgetCustomPlot::clearAll()
{
    for (int i = 0; i < maxTraces; i++)
        plot->graph(i)->data()->clear();
}

void WidgetCustomPlot::updateTrace(QVector<QPointF> *points, int index)
{    
    QVector<qreal> x, y;
    for(int i = 0; i < points->size(); i++){
        x.append(points->at(i).x());
        y.append(points->at(i).y());
    }

    plot->graph(index)->addData(x, y);
    plot->replot();
}

void WidgetCustomPlot::setRangeX(qreal minX, qreal maxX)
{    
    plot->xAxis->setRange(minX, maxX);
    plot->replot();
}

void WidgetCustomPlot::setRangeY(qreal minY, qreal maxY)
{
    plot->yAxis->setRange(minY, maxY);
    plot->replot();
}

void WidgetCustomPlot::setRange(qreal minX, qreal maxX, qreal minY, qreal maxY)
{
    plot->xAxis->setRange(minX, maxX);
    plot->yAxis->setRange(minY, maxY);
    plot->replot();
}

void WidgetCustomPlot::setTraceColor(int index, QColor color)
{
    plot->graph(index)->setPen(QPen(QBrush(color), 2));
}

void WidgetCustomPlot::setGridLinesVisible(bool gridVisibleX, bool gridVisibleY)
{
    plot->xAxis->grid()->setVisible(gridVisibleX);
    plot->yAxis->grid()->setVisible(gridVisibleY);
    plot->replot();
}

void WidgetCustomPlot::setGridDensity(int tickX, int tickY)
{
    plot->xAxis->ticker()->setTickCount(tickX);
    plot->yAxis->ticker()->setTickCount(tickY);
    plot->replot();
}

void WidgetCustomPlot::setGridHorizontalDensity(int tickX)
{    
    plot->xAxis->ticker()->setTickCount(tickX);
    plot->replot();
}

void WidgetCustomPlot::setGraphColor(QColor qColor)
{
    plot->xAxis->setLabelColor(qColor);
    plot->yAxis->setLabelColor(qColor);
    plot->xAxis->setTickLabelColor(qColor);
    plot->yAxis->setTickLabelColor(qColor);

    plot->xAxis->grid()->setPen(QPen(QBrush(qColor), 1));
    plot->yAxis->grid()->setPen(QPen(QBrush(qColor), 1));
    plot->xAxis->grid()->setZeroLinePen(QPen(QBrush(qColor), 1));
    plot->yAxis->grid()->setZeroLinePen(QPen(QBrush(qColor), 1));

    plot->xAxis->setBasePen(QPen(QBrush(qColor), 1));
    plot->yAxis->setBasePen(QPen(QBrush(qColor), 1));
    plot->xAxis->setTickPen(QPen(QBrush(qColor), 1));
    plot->yAxis->setTickPen(QPen(QBrush(qColor), 1));
}

void WidgetCustomPlot::setLabelsVisible(bool labelVisibleX, bool labelVisibleY)
{
    plot->xAxis->setTickLabels(labelVisibleX);
    plot->yAxis->setTickLabels(labelVisibleY);
}
