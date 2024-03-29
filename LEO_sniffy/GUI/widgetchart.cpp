#include "widgetchart.h"
#include "ui_widgetchart.h"

widgetChart::widgetChart(QWidget *parent, int maxTraces) :
    QWidget(parent),
    ui(new Ui::widgetChart),
    maxTraces(maxTraces)
{
    ui->setupUi(this);
    setStyleSheet("background-color:" + Graphics::COLOR_DATA_INPUT_AREA);

    chart = new QChart();
    chart->legend()->hide();
    chart->setObjectName("chart");

    chart->setBackgroundBrush(QColor(Graphics::COLOR_CHART));
    chart->setAcceptHoverEvents(true);
    this->setMouseTracking(true);

    QMargins *chrtMargin = new QMargins(0,0,0,0);
    chart->setMargins(*chrtMargin);

    axisX = new QValueAxis;
    axisY = new QValueAxis;

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    //init traces
    for (int i = 0; i < maxTraces; i++) {
        QLineSeries *series = new QLineSeries;
        //connect(series, &QSplineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(Graphics::getChannelColor(i))), 2.0));
        seriesList.append(series);
        createSeries(series);
    }

    formatAxisLabelsForScope();

    //init markers
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
        seriesH->attachAxis(axisX_UnitRange);
        seriesH->attachAxis(axisY);
        chart->addSeries(seriesV);
        seriesV->attachAxis(axisX);
        seriesV->attachAxis(axisY_UnitRange);
    }

    //init cursors
    for (int i = 0; i < 2; i++){
        QLineSeries *seriesCursor = new QLineSeries;
        cursorsHorizontal.append(seriesCursor);
        chart->addSeries(seriesCursor);
        seriesCursor->attachAxis(axisX);
        seriesCursor->attachAxis(axisY_UnitRange);
        connect(seriesCursor,&QLineSeries::pressed,this,&widgetChart::handleCursorPress);
        connect(seriesCursor,&QLineSeries::released,this,&widgetChart::handleCursorRelease);
    }
    for (int i = 0; i < 2; i++){
        QLineSeries *seriesCursor = new QLineSeries;
        cursorsVertical.append(seriesCursor);
        chart->addSeries(seriesCursor);
        seriesCursor->attachAxis(axisX_UnitRange);
        seriesCursor->attachAxis(axisY);
    }

    QChartView *chartView = new QChartView(chart);

    ui->horizontalLayout_chart->addWidget(chartView);
    chart->setMargins(QMargins(0,-12,0,0));
    chart->setBackgroundRoundness(0);

    setGraphColor(QColor(Graphics::COLOR_CHART_GRIDLEG_LOW_CONTRAST));
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
    chartMode = ChartMode::SPLINE;
    line->setChecked(false);
    spline->setChecked(true);
    scatter->setChecked(false);
    for (int i = 0; i < maxTraces; i++) {
        QSplineSeries *series = new QSplineSeries;
        //connect(series, &QSplineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(Graphics::getChannelColor(i))), 2.0));
        series->append(seriesList[i]->points());
        chart->removeSeries(seriesList[i]);
        seriesList[i]->clear();
        seriesList.replace(i, series);
        createSeries(series);
        series->clear();
    }
}

void widgetChart::switchToLineSeriesCallback(){
    chartMode = ChartMode::LINE;
    line->setChecked(true);
    spline->setChecked(false);
    scatter->setChecked(false);
    switchToLineSeriesSeamless();
}

void widgetChart::switchToLineSeriesSeamless()
{
    for (int i = 0; i < maxTraces; i++) {
        QLineSeries *series = new QLineSeries;
        //connect(series, &QLineSeries::hovered, this, &widgetChart::hovered);
        series->setPen(QPen(QBrush(QColor(Graphics::getChannelColor(i))), 2.0));
        series->append(seriesList[i]->points());
        chart->removeSeries(seriesList[i]);
        seriesList[i]->clear();
        seriesList.replace(i, series);
        createSeries(series);
        series->clear();
    }
}

void widgetChart::switchToScatterSeriesCallback(){
    line->setChecked(false);
    spline->setChecked(false);
    scatter->setChecked(true);
    chartMode = ChartMode::SCATTER;
    for (int i = 0; i < maxTraces; i++) {
        QScatterSeries *series = new QScatterSeries;
        //connect(series, &QScatterSeries::hovered, this, &widgetChart::hovered);
        series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        series->setMarkerSize(20.0);
        series->setPen(QColor(Qt::transparent));
        series->setBrush(getBrush(i,MarkerType::CIRCLE));
        series->append(seriesList[i]->points());
        chart->removeSeries(seriesList[i]);
        seriesList[i]->clear();
        seriesList.replace(i, series);
        createSeries(series);
        series->clear();
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


bool widgetChart::eventFilter(QObject *obj, QEvent *event)
{    
    Q_UNUSED(obj);

    if(eventSel != EventSelection::CLICKS_ONLY){
        if(event->type() == QEvent::GraphicsSceneWheel){ //zoom by wheel
            QGraphicsSceneWheelEvent *ev = (QGraphicsSceneWheelEvent*) event;
            qreal tmp = ev->delta();
            if(tmp>=0){
                localZoom = localZoom *qPow(1.001,tmp);
            }else{
                localZoom = localZoom / qPow(1.001,-tmp);
                if(localZoom<1)
                    localZoom=1;
            }
            emit localZoomChanged();
            updateAxis();
        }

        if(event->type() == QEvent::GraphicsSceneMouseDoubleClick){ //restore zoom on double click
            localZoom = 1;
            shift = abs(minX)/(maxX-minX);
            if(shift>1)shift = 1;
            if(shift<0)shift = 0;
            emit localZoomChanged();
            updateAxis();
        }

        if(event->type() == QEvent::GraphicsSceneMouseMove){ //move signal when dragged

            QGraphicsSceneMouseEvent *ev = (QGraphicsSceneMouseEvent*) event;
            qreal distance = ((ev->pos().x()- mousePressedPoint.x())/chart->geometry().width())/(localZoom/invZoom);
            shift = initMouseShift - distance;
            if(shift>1){
                shift = 1;
            }else if(shift<0){
                shift = 0;
            }
            emit localZoomChanged();
            updateAxis();
        }
    }

    if(event->type() == QEvent::GraphicsSceneMousePress){ //drag signal
        QGraphicsSceneMouseEvent *ev = (QGraphicsSceneMouseEvent*) event;
        mousePressed = true;
        mousePressedPoint = ev->pos();
        initMouseShift = shift;
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
        while (mousePressed) {
            Timing *timer = new Timing();
            timer->sleep(100);
        }
    }

    if(event->type() == QEvent::GraphicsSceneMouseRelease){
        mousePressed = false;
        QGraphicsSceneMouseEvent *ev = (QGraphicsSceneMouseEvent*) event;
        emit mouseLeftClickEvent(ev);
        QApplication::restoreOverrideCursor();
    }

    if(event->type() == QEvent::GraphicsSceneHoverEnter){
        QApplication::setOverrideCursor(QCursor(Qt::OpenHandCursor));
    }

    if(event->type() == QEvent::GraphicsSceneHoverLeave){
        QApplication::restoreOverrideCursor();
    }

    if(event->type() != QEvent::LayoutRequest && event->type()!=QEvent::GraphicsSceneHoverMove){
        // qDebug() <<"Sereies event"<< event->type() << obj->objectName();
    }
    return QObject::eventFilter(obj, event);
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
    qreal subMax = (maxX - minX) * (1-invZoom/localZoom);
    qreal tmpMin = subMax * shift+minX;
    qreal tmpMax = (maxX-minX)*invZoom/localZoom+tmpMin;
    axisX->setRange(tmpMin,tmpMax);

    /*  if(!seriesList.isEmpty() && seriesList.at(0)->count()>1){
        qreal percent = (tmpMax-tmpMin)/(seriesList.at(0)->points().last().x()-seriesList.at(0)->points().first().x());
        int samplesshown = percent*seriesList.at(0)->count();
       // qDebug() << "Update axis " +QString::number(samplesshown)<<"points";
        if(samplesshown<NUM_SAMPLES_TO_SWITCH){
            switch (chartMode) {
            case ChartMode::LINE:
                switchToLineSeriesCallback();
              //  qDebug() << "LINE";
                break;
            case ChartMode::SPLINE:
                switchToSplineSeriesCallback();
              //  qDebug() << "SPLINE";
                break;
            case ChartMode::SCATTER:
                switchToScatterSeriesCallback();
               // qDebug() << "SCATTER";
                break;
            }
        }else{
            switchToLineSeriesSeamless(); //todo no need to call this every time
        }
    }*/
}

void widgetChart::hideAxislabels()
{
    axisX->setLabelsVisible(false);
    axisY->setLabelsVisible(false);
}

void widgetChart::setDataMinMax(qreal minX, qreal maxX){
    this->minX = minX;
    this->maxX = maxX;

    if(lastMaxX!=maxX || lastMinX!=minX){
        lastMaxX = maxX;
        lastMinX = minX;
        updateAxis();
    }
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

qreal widgetChart::getSignalValue(int traceIndex, qreal time)
{
    qreal closestSmaller = -10000;
    int closestSmallerIndex = 0;
    for (int i =0;i<seriesList[traceIndex]->count();i++) {
        if (seriesList[traceIndex]->at(i).x() < time && seriesList[traceIndex]->at(i).x()>closestSmaller ){
            closestSmaller = seriesList[traceIndex]->at(i).x();
            closestSmallerIndex = i;
        }
    }
    if (closestSmallerIndex == seriesList[traceIndex]->count()-1){
        return seriesList[traceIndex]->at(closestSmallerIndex).y();
    }
    qreal divY = seriesList[traceIndex]->at(closestSmallerIndex+1).y()-seriesList[traceIndex]->at(closestSmallerIndex).y();
    qreal divX = seriesList[traceIndex]->at(closestSmallerIndex+1).x()-seriesList[traceIndex]->at(closestSmallerIndex).x();
    qreal tmp = seriesList[traceIndex]->at(closestSmallerIndex).y()+(time-seriesList[traceIndex]->at(closestSmallerIndex).x())/divX*divY;
    return tmp;
}

void widgetChart::setMargins(int left, int top, int right, int bottom){
    QMargins *chrtMargin = new QMargins(left, top, right, bottom);
    chart->setMargins(*chrtMargin);
}

void widgetChart::setZoom(float zoom){
    this->invZoom = 1/zoom;
    if(invZoom!=lastInvZoom){
        lastInvZoom = invZoom;
        updateAxis();
    }
}

qreal widgetChart::getZoom(){
    return 1/invZoom;
}

qreal widgetChart::getLocalZoom(){
    return localZoom;
}

void widgetChart::setLocalZoom(const qreal &value)
{
    localZoom = value;
}

/* range -100 to 100 (represents percent value) */
void widgetChart::setShift (float shift){
    this->shift = (shift/2+50)/100;
    updateAxis();
}

qreal widgetChart::getShift()
{
    return shift;
}

void widgetChart::enableLocalMouseEvents(EventSelection sel)
{
    chart->installEventFilter(this);
    eventSel = sel;
}

void widgetChart::setGridLinesVisible(bool gridVisibleX, bool gridVisibleY){
    axisX->setGridLineVisible(gridVisibleX);
    axisY->setGridLineVisible(gridVisibleY);
}

void widgetChart::setGridDensity(int tickX, int tickY){
    axisX->setTickCount(tickX);
    axisY->setTickCount(tickY);
}

void widgetChart::setGridHorizontalDensity(int tickX){
    axisX->setTickCount(tickX);
}

void widgetChart::setGraphColor(QColor qColor){
    axisX->setLabelsBrush(QBrush(qColor));
    axisY->setLabelsBrush(QBrush(qColor));
    axisY_UnitRange->setLinePenColor(qColor);
    axisX_UnitRange->setLinePenColor(qColor);
    axisX->setGridLineColor(qColor);
    axisY->setGridLineColor(qColor);
    axisX->setLinePenColor(qColor);
    axisY->setLinePenColor(qColor);
}

void widgetChart::setTraceColor(int index, QColor color){
    seriesList.at(index)->setColor(color);
}

void widgetChart::formatAxisLabelsForScope(){
    axisX->setLabelsBrush(QBrush(QColor(128,128,128)));
    axisY->setLabelsBrush(QBrush(QColor(128,128,128)));
    axisX->setLabelFormat("%.2g");
    //axisY->setLabelFormat("%.2g");

    QFont font = axisX->labelsFont();
    font.setPointSize(8);
    axisX->setLabelsFont(font);
    axisY->setLabelsFont(font);
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
    axisX_UnitRange = new QValueAxis;
    axisX_UnitRange->setRange(0, 1);
    axisX_UnitRange->setLabelsVisible(false);
    axisX_UnitRange->setTickCount(2);
    chart->addAxis(axisX_UnitRange, Qt::AlignTop);
}

void widgetChart::createVerticalMarkers(){
    //prepare markers on X axis
    axisY_UnitRange = new QValueAxis;
    axisY_UnitRange->setRange(0, 1);
    axisY_UnitRange->setLabelsVisible(false);
    axisY_UnitRange->setTickCount(2);
    chart->addAxis(axisY_UnitRange, Qt::AlignRight);
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

    MarkerPath_Trigger = new QPainterPath(QPointF(0,5));
    MarkerPath_Trigger->lineTo(15,5);
    MarkerPath_Trigger->lineTo(20,10);
    MarkerPath_Trigger->lineTo(15,15);

    MarkerPath_Trigger->lineTo(13,15);
    MarkerPath_Trigger->lineTo(13,10);
    MarkerPath_Trigger->lineTo(16,10);
    MarkerPath_Trigger->lineTo(16,7);
    MarkerPath_Trigger->lineTo(7,7);
    MarkerPath_Trigger->lineTo(7,10);
    MarkerPath_Trigger->lineTo(10,10);
    MarkerPath_Trigger->lineTo(10,15);

    MarkerPath_Trigger->lineTo(0,15);

}

QBrush widgetChart::getBrush(int channelIndex, MarkerType type)
{
    QImage marker(20, 20, QImage::Format_ARGB32);
    marker.fill(Qt::transparent);

    QPainter painter(&marker);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QBrush(QColor(Graphics::getChannelColor(channelIndex))), 1.0));
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
        painter.setPen(QPen(QBrush(QColor(Graphics::getChannelColor(channelIndex))), 2.0));
        painter.drawPath(*MarkerPath_Cross);
        break;
    case MarkerType::CIRCLE:
        painter.drawPath(*MarkerPath_Circle);
        break;
    case MarkerType::TRIGGER:
        painter.drawPath(*MarkerPath_Trigger);
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
    triggerShift = value;
    QList<QPointF> *lst = new QList<QPointF>;
    lst->append(pt);
    markersVertical[markerVerticalIndex]->setBrush(getBrush(channelIndex,MarkerType::ARROW_DOWN_BIG));
    markersVertical[markerVerticalIndex]->replace(*lst);
    markerVerticalIndex++;
}

void widgetChart::setHorizontalCursor(int channelIndex, qreal value, Cursor type)
{
    QPointF start = QPointF(value,0.0);
    QPointF end = QPointF(value,1.0);
    QList<QPointF> *lst = new QList<QPointF>;
    lst->append(start);
    lst->append(end);

    QPen *pen = new QPen();
    pen->setColor(Graphics::getChannelColor(channelIndex));
    pen->setWidth(2);
    if(type == Cursor::CURSOR_A){
        pen->setStyle(Qt::DashLine);
        cursorsHorizontal[0]->setPen(*pen);
        cursorsHorizontal[0]->replace(*lst);
    }else{
        pen->setStyle(Qt::DashDotLine);
        cursorsHorizontal[1]->setPen(*pen);
        cursorsHorizontal[1]->replace(*lst);
    }
}

void widgetChart::setVerticalCursor(int channelIndex, qreal value, Cursor type)
{
    QPointF start = QPointF(0.0,value);
    QPointF end = QPointF(1.0,value);
    QList<QPointF> *lst = new QList<QPointF>;
    lst->append(start);
    lst->append(end);

    QPen *pen = new QPen();
    pen->setColor(Graphics::getChannelColor(channelIndex));
    pen->setWidth(2);

    if(type == Cursor::CURSOR_A){
        pen->setStyle(Qt::DashLine);
        cursorsVertical[0]->setPen(*pen);
        cursorsVertical[0]->replace(*lst);
    }else{
        pen->setStyle(Qt::DashDotLine);
        cursorsVertical[1]->setPen(*pen);
        cursorsVertical[1]->replace(*lst);
    }
}

void widgetChart::clearAllCursors()
{
    cursorsVertical[0]->clear();
    cursorsVertical[1]->clear();
    cursorsHorizontal[0]->clear();
    cursorsHorizontal[1]->clear();
}

void widgetChart::initContextMenu(){
    menu = new QMenu(tr("Context menu"), this);

    spline = new QAction("Spline", this);
    spline->setCheckable(true);
    spline->setChecked(true);
    line = new QAction("Line", this);
    line->setCheckable(true);
    line->setChecked(false);
    scatter  = new QAction("Points", this);
    scatter->setCheckable(true);
    scatter->setChecked(false);
    /* btnOpenGL = new QAction("Use OpenGL", this);
    btnOpenGL->setCheckable(true);
    btnOpenGL->setChecked(true);*/

    connect(spline, SIGNAL(triggered()), this, SLOT(switchToSplineSeriesCallback()));
    connect(line, SIGNAL(triggered()), this, SLOT(switchToLineSeriesCallback()));
    connect(scatter, SIGNAL(triggered()), this, SLOT(switchToScatterSeriesCallback()));
    //connect(btnOpenGL, SIGNAL(triggered()), this, SLOT(useOpenGLCallback()));

    menu->addAction(spline);
    menu->addAction(line);
    menu->addAction(scatter);
    //menu->addAction(btnOpenGL);
}

void widgetChart::rightClickCallback(const QPoint &mousePos){
    menu->exec(mapToGlobal(mousePos));
}

void widgetChart::hovered(const QPointF &point){
    QString coords = QString("X: %1\nY: %2").arg(point.x()).arg(point.y());
    chart->setToolTip(coords);
}

void widgetChart::handleCursorPress()
{
    QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
    qDebug () << "cursor clicked";
}

void widgetChart::handleCursorRelease()
{
    QApplication::restoreOverrideCursor();
}
