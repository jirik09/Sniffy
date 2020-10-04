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

    chart->setBackgroundBrush(BACKGROUND_QCOLOR_DATA_AREA);
    chart->setAcceptHoverEvents(true);
    this->setMouseTracking(true);
    //chart->setToolTip("¯\\_(ツ)_/¯");

    QMargins *chrtMargin = new QMargins(0,5,0,0);
    chart->setMargins(*chrtMargin);

    axisX = new QValueAxis;
    axisY = new QValueAxis;

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (int i = 0; i < maxTraces; i++) {
        QSplineSeries *series = new QSplineSeries;
        //connect(series, &QSplineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(Colors::getChannelColor(i)), 2.0));
        seriesList.append(series);
        createSeries(series);
    }

    formatAxisLabelsForScope();

    createHorizontalMarkers();
    createVerticalMarkers();
    initBrushes();
    for (int i = 0; i < maxTraces*2; i++) {
        QScatterSeries *seriesH = new QScatterSeries;
        QScatterSeries *seriesV = new QScatterSeries;

        markersVertical.append(seriesV);
        markersHorizontal.append(seriesH);

        seriesH->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        seriesH->setMarkerSize(20.0);
        seriesH->setPen(QColor(Qt::transparent));

        seriesV->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        seriesV->setMarkerSize(20.0);
        seriesV->setPen(QColor(Qt::transparent));

        chart->addSeries(seriesH);
        seriesH->attachAxis(axisX_MarkerHorizontal);
        seriesH->attachAxis(axisY);
        chart->addSeries(seriesV);
        seriesV->attachAxis(axisX);
        seriesV->attachAxis(axisY_MarkerVertical);
    }

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
        series->setPen(QPen(QBrush(Colors::getChannelColor(i)), 2.0));

        series->append(seriesList[i]->points());
        seriesList[i]->clear();
        seriesList.replace(i, series);

        createSeries(series);
        series->setUseOpenGL(true);
    }
}

void widgetChart::switchToLineSeriesCallback(){
    for (int i = 0; i < maxTraces; i++) {
        QLineSeries *series = new QLineSeries;
        //connect(series, &QLineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(Colors::getChannelColor(i)), 2.0));

        series->append(seriesList[i]->points());
        seriesList[i]->clear();
        seriesList.replace(i, series);

        createSeries(series);
        series->setUseOpenGL(true);
    }
}

void widgetChart::switchToScatterSeriesCallback(){
    for (int i = 0; i < maxTraces; i++) {
        QScatterSeries *series = new QScatterSeries;
        //connect(series, &QScatterSeries::hovered, this, &widgetChart::hovered);
        series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        series->setMarkerSize(20.0);
        series->setPen(QColor(Qt::transparent));

        series->setBrush(getBrush(i,MarkerType::CIRCLE));
        series->append(seriesList[i]->points());
        seriesList[i]->clear();
        seriesList.replace(i, series);

        createSeries(series);
        series->setUseOpenGL(false);
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
    for (int i = 0; i < maxTraces; i++){
        seriesList[i]->clear();
    }

    for (int i = 0; i < maxTraces*2; i++){
        markersHorizontal[i]->clear();
        markersVertical[i]->clear();
    }
    markerHorizontalIndex = 0;
    markerVerticalIndex = 0;
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
    axisY_MarkerVertical->setLinePenColor(qColor);
    axisX_MarkerHorizontal->setLinePenColor(qColor);
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

void widgetChart::createHorizontalMarkers(){
    //prepare markers on Y axis
    axisX_MarkerHorizontal = new QValueAxis;
    axisX_MarkerHorizontal->setRange(0, 1);
    axisX_MarkerHorizontal->setLabelsVisible(false);
    axisX_MarkerHorizontal->setTickCount(2);
    chart->addAxis(axisX_MarkerHorizontal, Qt::AlignTop);
}

void widgetChart::createVerticalMarkers(){
    //prepare markers on X axis
    axisY_MarkerVertical = new QValueAxis;
    axisY_MarkerVertical->setRange(0, 1);
    axisY_MarkerVertical->setLabelsVisible(false);
    axisY_MarkerVertical->setTickCount(2);
    chart->addAxis(axisY_MarkerVertical, Qt::AlignRight);
}

void widgetChart::initBrushes()
{
    MarkerPath_ArrowDownBig = new QPainterPath();
    MarkerPath_ArrowDownBig->lineTo(10,20);
    MarkerPath_ArrowDownBig->lineTo(20,0);

    MarkerPath_Tick = new QPainterPath(QPoint(0,7));
    MarkerPath_Tick->lineTo(14,7);
    MarkerPath_Tick->lineTo(19,10);
    MarkerPath_Tick->lineTo(14,13);
    MarkerPath_Tick->lineTo(0,13);

    MarkerPath_ArrowDownSmall = new QPainterPath(QPointF(5,0));
    MarkerPath_ArrowDownSmall->lineTo(10,10);
    MarkerPath_ArrowDownSmall->lineTo(15,0);

    MarkerPath_ArrowUpSmall = new QPainterPath(QPointF(5,20));
    MarkerPath_ArrowUpSmall->lineTo(10,10);
    MarkerPath_ArrowUpSmall->lineTo(15,20);


    MarkerPath_Cross = new QPainterPath(QPointF(7,7));
    MarkerPath_Cross->lineTo(13,13);
    MarkerPath_Cross->moveTo(7,13);
    MarkerPath_Cross->lineTo(13,7);

    MarkerPath_Circle = new QPainterPath(QPointF(10,10));
    MarkerPath_Circle->arcTo(QRectF(8,8,4,4),0,360*16);


}

QBrush widgetChart::getBrush(int channelIndex, MarkerType type)
{
    QImage marker(20, 20, QImage::Format_ARGB32);
    marker.fill(Qt::transparent);

    QPainter painter(&marker);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QBrush(Colors::getChannelColor(channelIndex)), 1.0));
    painter.setBrush(painter.pen().color());
    switch (type) {
    case MarkerType::ARROW_DOWN_BIG:
        painter.drawPath(*MarkerPath_ArrowDownBig);
        break;
    case MarkerType::ARROW_UP_SMALL:
        painter.drawPath(*MarkerPath_ArrowUpSmall);
        break;
    case MarkerType::ARROW_DOWN_SMALL:
        painter.drawPath(*MarkerPath_ArrowDownSmall);
        break;
    case MarkerType::TICK:
        painter.drawPath(*MarkerPath_Tick);
        break;
    case MarkerType::CROSS:
        painter.setPen(QPen(QBrush(Colors::getChannelColor(channelIndex)), 2.0));
        painter.drawPath(*MarkerPath_Cross);
        break;
    case MarkerType::CIRCLE:
        painter.drawPath(*MarkerPath_Circle);
        break;
    }

    return marker;
}

void widgetChart::setHorizontalMarker(int channelIndex, qreal value, MarkerType type){
    QPointF pt = QPointF(0.005,value);
    QList<QPointF> *lst = new QList<QPointF>;
    lst->append(pt);

    markersHorizontal[markerHorizontalIndex]->setBrush(getBrush(channelIndex,type));
    markersHorizontal[markerHorizontalIndex]->replace(*lst);
    markerHorizontalIndex++;
}

void widgetChart::setVerticalMarker(int channelIndex, qreal value){

    QPointF pt = QPointF(value,0.99);
    QList<QPointF> *lst = new QList<QPointF>;
    lst->append(pt);

    markersVertical[markerVerticalIndex]->setBrush(getBrush(channelIndex,MarkerType::ARROW_DOWN_BIG));
    markersVertical[markerVerticalIndex]->replace(*lst);
    markerVerticalIndex++;
}

void widgetChart::initContextMenu(){
    menu = new QMenu(tr("Context menu"), this);
    menu->setStyleSheet(CONTEXT_MENU_HOVER);

    spline = new QAction("Spline", this);
    line = new QAction("Line", this);
    scatter  = new QAction("Points (slow)", this);
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
