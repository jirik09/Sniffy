#include "widgetchart.h"
#include "ui_widgetchart.h"

widgetChart::widgetChart(QWidget *parent, int maxTraces) :
    QWidget(parent),
    ui(new Ui::widgetChart)
{
    //QStringList colors;



    this->maxTraces = maxTraces;

    ui->setupUi(this);
    chart = new QChart();
    chart->legend()->hide();

    QMargins *chrtMargin = new QMargins(0,5,0,0);

    chart->setMargins(*chrtMargin);

    axisX = new QValueAxis;
    axisY = new QValueAxis;


    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);


    colors[0] = QColor(255,170,0);
    colors[1] = QColor(24,154,224);
    colors[2] = QColor(54,154,24);
    colors[3] = QColor(204,24,224);

    axisX->setRange(-0.1, 0.1);
    axisY->setRange(-0.1, 3.9);



    for (int i = 0; i < maxTraces; i++) {
        QSplineSeries *series = 0; //spline
        series = new QSplineSeries;

      /*  QXYSeries *series = 0; //normal
        series = new QLineSeries;*/

        series->setPen(QPen(QBrush(QColor(colors[i%5])), 2.0));

        seriesList.append(series);
        series->setUseOpenGL(true);
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    //prepare markers on X axis
    axisMarkerHorizontal = new QValueAxis;
    chart->addAxis(axisMarkerHorizontal, Qt::AlignRight);
    axisMarkerHorizontal->setRange(0, 1);
    markersHorizontal = new QScatterSeries;

    markersHorizontal->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    markersHorizontal->setMarkerSize(20.0);
    markersHorizontal->setPen(QColor(Qt::transparent));
   // markersHorizontal->setUseOpenGL(true);
    chart->addSeries(markersHorizontal);
    markersHorizontal->attachAxis(axisX);
    markersHorizontal->attachAxis(axisMarkerHorizontal);


    QChartView *chartView = new QChartView(chart);
    chartView->setMinimumSize(200,100);
        //chartView->setStyleSheet(QString::fromUtf8("background-color: rgb(155, 155, 155);color: rgb(255, 255, 255);"));


        // Customize plot area background

        chart->setBackgroundBrush(QColor(38,38,38));

        /*QPen axisPen(QRgb(0xf0f0f0));
        axisPen.setWidth(2);
        axisX->setLinePen(axisPen);
        axisY->setLinePen(axisPen);*/

        axisX->setLabelsBrush(QBrush(QColor(128,128,128)));
        axisX->setLabelFormat("%.1e");
        axisY->setLabelsBrush(QBrush(QColor(128,128,128)));
        axisY->setLabelFormat("%.2g");

        axisMarkerHorizontal->setLabelsVisible(false);

        QFont font = axisX->labelsFont();
        font.setPointSize(8);
        axisX->setLabelsFont(font);
        axisY->setLabelsFont(font);

       // axisX->setMinorGridLineVisible(true);
        axisX->setTickCount(11);
        axisY->setTickCount(9);


      //  axisX->setLabelsVisible(false);


       // axisX->setShadesVisible(false);

        //axisX->setLabelsBrush(axisBrush);
        //axisY->setLabelsBrush(axisBrush);
        ui->horizontalLayout_chart->addWidget(chartView);

}

widgetChart::~widgetChart()
{
    delete ui;
}

void widgetChart::clearAll(){
    for (int i = 0; i < maxTraces; i++) {
        seriesList[i]->clear();
    }
    markersHorizontal->clear();
}

void widgetChart::updateTrace(QVector<QPointF> *points, int index){
    const QVector<QPointF> &seriesData = *points;
    seriesList[index]->replace(seriesData);
}

void widgetChart::updateAxis(){
    qreal subMax = (maxX - minX) * (1-invZoom);
    qreal tmpMin = subMax * shift+minX;
    qreal tmpMax = (maxX-minX)*invZoom+tmpMin;
    axisX->setRange(tmpMin,tmpMax);
}

void widgetChart::setMaxX(float max){
    axisX->setRange(0, max);
}

void widgetChart::setDataMinMax(qreal minX, qreal maxX){
    this->minX = minX;
    this->maxX = maxX;
    updateAxis();
}

void widgetChart::setZoom(float zoom){
    this->invZoom = 1/zoom;
    updateAxis();
}

qreal widgetChart::getZoom()
{
    return 1/invZoom;
}

/*
range -100 to 100 (represents percent value)
*/
void widgetChart::setShift (float shift){
    this->shift = (shift/2+50)/100;
    updateAxis();
}

void widgetChart::setMarkerHorizontal(int channelIndex, qreal value){
    QVector<QPointF> seriesData;
    QPainterPath arrowPath;
    arrowPath.lineTo(10,20);
    arrowPath.lineTo(20,0);
    arrowPath.lineTo(0,0);
    QImage arrow(20, 20, QImage::Format_ARGB32);
    arrow.fill(Qt::transparent);

    QPainter painter(&arrow);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QBrush(QColor(colors[channelIndex%5])), 1.0));
    painter.setBrush(painter.pen().color());
    painter.drawPath(arrowPath);

    markersHorizontal->setBrush(arrow);
    seriesData.append(QPointF(value,0.99));
    markersHorizontal->replace(seriesData);
}
