#include "widgetchart.h"
#include "ui_widgetchart.h"

widgetChart::widgetChart(QWidget *parent, int maxTraces) :
    QWidget(parent),
    ui(new Ui::widgetChart),
    maxTraces(maxTraces)
{
    ui->setupUi(this);

    chart = new QChart();
    chart->legend()->hide();

    chart->setBackgroundBrush(QCOLOR_BLACK);
    chart->setAcceptHoverEvents(true);
    this->setMouseTracking(true);
    chart->setToolTip("¯\\_(ツ)_/¯");

    QMargins *chrtMargin = new QMargins(0,5,0,0);
    chart->setMargins(*chrtMargin);

    axisX = new QValueAxis;
    axisY = new QValueAxis;

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (int i = 0; i < maxTraces; i++) {
        QSplineSeries *series = new QSplineSeries;
        //connect(series, &QSplineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(colors[i%5])), 2.0));
        seriesList.append(series);
        createSeries(series);
    }

    formatAxisLabelsForScope();

    QChartView *chartView = new QChartView(chart);
    ui->horizontalLayout_chart->addWidget(chartView);

    initContextMenu();
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(rightClickCallback(const QPoint &)));
}

widgetChart::~widgetChart()
{
    delete ui;
}

void widgetChart::switchToSplineSeriesCallback(){
    for (int i = 0; i < maxTraces; i++) {
        QSplineSeries *series = new QSplineSeries;
        //connect(series, &QSplineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(colors[i%5])), 2.0));

        series->append(seriesList[i]->points());
        seriesList[i]->clear();
        seriesList.replace(i, series);

        createSeries(series);
    }
}

void widgetChart::switchToLineSeriesCallback(){
    for (int i = 0; i < maxTraces; i++) {
        QLineSeries *series = new QLineSeries;
        //connect(series, &QLineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(colors[i%5])), 2.0));

        series->append(seriesList[i]->points());
        seriesList[i]->clear();
        seriesList.replace(i, series);

        createSeries(series);
    }
}

void widgetChart::switchToScatterSeriesCallback(){
    for (int i = 0; i < maxTraces; i++) {
        QScatterSeries *series = new QScatterSeries;
        //connect(series, &QScatterSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(colors[i%5])), 2.0));
        series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series->setMarkerSize(3.8);

        series->append(seriesList[i]->points());
        seriesList[i]->clear();
        seriesList.replace(i, series);

        createSeries(series);
    }
}

void widgetChart::useOpenGLCallback(){
    bool use = btnOpenGL->isChecked();
    btnOpenGL->setChecked(use);
    for (int i = 0; i < maxTraces; i++) {
        chart->series().at(i)->setUseOpenGL(use);
    }
}

void widgetChart::createSeries(QAbstractSeries *series){
    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setUseOpenGL(true);
}

void widgetChart::clearAll(){
    for (int i = 0; i < maxTraces; i++)
        seriesList[i]->clear();
    if(markers)
        markersHorizontal->clear();
}

void widgetChart::clearPoints(int startIndex, int endIndex){
    for (int i = startIndex; i <= endIndex; i++)
        seriesList[i]->clear();
}

void widgetChart::clearPoint(int traceIndex, int index){
    if(seriesList[traceIndex]->count() != 0)
        seriesList[traceIndex]->remove(index);
}

void widgetChart::updateTrace(QVector<QPointF> *points, int index){
    const QVector<QPointF> &seriesData = *points;
    seriesList[index]->replace(seriesData);
}

void widgetChart::appendToTrace(int index, QVector<QPointF> *points){
    seriesList[index]->append(points->at(points->length()-1));
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

void widgetChart::setRangeX(qreal minX, qreal maxX){
    axisX->setRange(minX, maxX);
}

void widgetChart::setRangeY(qreal minY, qreal maxY){
    axisY->setRange(minY, maxY);
}

void widgetChart::setRange(qreal minX, qreal maxX, qreal minY, qreal maxY){
    axisX->setRange(minX, maxX);
    axisY->setRange(minY, maxY);
}

void widgetChart::setMargins(int left, int top, int right, int bottom){
    QMargins *chrtMargin = new QMargins(left, top, right, bottom);
    chart->setMargins(*chrtMargin);
}

void widgetChart::setZoom(float zoom){
    this->invZoom = 1/zoom;
    updateAxis();
}

qreal widgetChart::getZoom(){
    return 1/invZoom;
}

/* range -100 to 100 (represents percent value) */
void widgetChart::setShift (float shift){
    this->shift = (shift/2+50)/100;
    updateAxis();
}

void widgetChart::setGridLinesVisible(bool gridVisibleX, bool gridVisibleY){
    axisX->setGridLineVisible(gridVisibleX);
    axisY->setGridLineVisible(gridVisibleY);
}

void widgetChart::setGridDensity(int tickX, int tickY){
    axisX->setTickCount(tickX);
    axisY->setTickCount(tickY);
}

void widgetChart::setGraphColor(QColor qColor){
    axisX->setLabelsBrush(QBrush(qColor));
    axisY->setLabelsBrush(QBrush(qColor));
    axisX->setGridLineColor(qColor);
    axisY->setGridLineColor(qColor);
    axisX->setLinePenColor(qColor);
    axisY->setLinePenColor(qColor);
}

void widgetChart::formatAxisLabelsForScope(){
    axisX->setLabelsBrush(QBrush(QColor(128,128,128)));
    axisY->setLabelsBrush(QBrush(QColor(128,128,128)));
    axisX->setLabelFormat("%.2g");
    axisY->setLabelFormat("%.2g");

    QFont font = axisX->labelsFont();
    font.setPointSize(8);
    axisX->setLabelsFont(font);
    axisY->setLabelsFont(font);
    axisX->setTickCount(11);
    axisY->setTickCount(9);
}

void widgetChart::formatLabels(QString axisXLabelForm, QString axisYLabelForm){
    axisX->setLabelFormat(axisXLabelForm);
    axisY->setLabelFormat(axisYLabelForm);
}

void widgetChart::setLabelsVisible(bool lableVisibleX, bool lableVisibleY){
    axisX->setLabelsVisible(lableVisibleX);
    axisY->setLabelsVisible(lableVisibleY);
}

void widgetChart::setLabelsSize(int pointSize){
    QFont font = axisX->labelsFont();
    font.setPointSize(pointSize);
    axisX->setLabelsFont(font);
    axisY->setLabelsFont(font);
}

void widgetChart::createHorizontalMarkes(){
    markers = true;
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

    axisMarkerHorizontal->setLabelsVisible(false);
}

void widgetChart::setHorizontalMarker(int channelIndex, qreal value){
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

int widgetChart::getTraceNum(){
    return maxTraces;
}

void widgetChart::initContextMenu(){
    menu = new QMenu(tr("Context menu"), this);
    menu->setStyleSheet(CONTEXT_MENU_HOVER);

    spline = new QAction("Spline", this);
    line = new QAction("Line", this);
    scatter  = new QAction("Scatter", this);
    btnOpenGL = new QAction("Use OpenGL", this);
    btnOpenGL->setCheckable(true);
    btnOpenGL->setChecked(true);

    connect(spline, SIGNAL(triggered()), this, SLOT(switchToSplineSeriesCallback()));
    connect(line, SIGNAL(triggered()), this, SLOT(switchToLineSeriesCallback()));
    connect(scatter, SIGNAL(triggered()), this, SLOT(switchToScatterSeriesCallback()));
    connect(btnOpenGL, SIGNAL(triggered()), this, SLOT(useOpenGLCallback()));

    menu->addAction(spline);
    menu->addAction(line);
    menu->addAction(scatter);
    menu->addAction(btnOpenGL);
}

void widgetChart::rightClickCallback(const QPoint &mousePos){
    menu->exec(mapToGlobal(mousePos));
}

void widgetChart::hovered(const QPointF &point){
    QString coords = QString("X: %1\nY: %2").arg(point.x()).arg(point.y());
    chart->setToolTip(coords);
}
