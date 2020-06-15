#include "widgetchart.h"
#include "ui_widgetchart.h"

widgetChart::widgetChart(QWidget *parent, int maxTraces) :
    QWidget(parent),
    ui(new Ui::widgetChart)
{
    //QStringList colors;
    QColor colors[4];
    colors[0] = QColor(255,170,0);
    colors[1] = QColor(24,154,224);
    colors[2] = QColor(54,154,24);
    colors[3] = QColor(204,24,224);

    this->maxTraces = maxTraces;

    ui->setupUi(this);
    QChart *chart = new QChart();
    chart->legend()->hide();

    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);


    for (int i = 0; i < maxTraces; i++) {
        QSplineSeries *series = 0;

        series = new QSplineSeries;
        series->setPen(QPen(QBrush(QColor(colors[i%5])), 3.0));

        seriesList.append(series);
        series->setUseOpenGL(true);
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    axisX->setRange(0, 0.1);
    axisY->setRange(0, 4.0);

    QChartView *chartView = new QChartView(chart);
    chartView->setMinimumSize(500,250);
        //chartView->setStyleSheet(QString::fromUtf8("background-color: rgb(155, 155, 155);color: rgb(255, 255, 255);"));


        // Customize plot area background

        chart->setBackgroundBrush(QColor(38,38,38));

      /*  QPen axisPen(QRgb(0xf0f0f0));
        axisPen.setWidth(2);
        axisX->setLinePen(axisPen);
        axisY->setLinePen(axisPen);*/

       // axisX->setMinorGridLineVisible(true);
        axisX->setTickCount(11);
        axisY->setTickCount(9);


        axisX->setLabelsVisible(false);
        axisY->setLabelsVisible(false);

        //axisX->setLabelsBrush(axisBrush);
        //axisY->setLabelsBrush(axisBrush);
        ui->horizontalLayout_chart->addWidget(chartView);

}

widgetChart::~widgetChart()
{
    delete ui;
}

void widgetChart::clearAll(){
    seriesList.clear();
}

void widgetChart::updateTrace(QVector<QPointF> *points, int index){
    const QVector<QPointF> &seriesData = *points;
    seriesList[index]->replace(seriesData);
}

void widgetChart::repaint(){

}
