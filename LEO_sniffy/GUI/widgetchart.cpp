#include "widgetchart.h"
#include "ui_widgetchart.h"
#include <QTimer>
//#include <QGraphicsSimpleTextItem>

widgetChart::widgetChart(QWidget *parent, int maxTraces) :
    QWidget(parent),
    ui(new Ui::widgetChart),
    maxTraces(maxTraces)
{
    ui->setupUi(this);
    setStyleSheet("background-color:" + Graphics::palette().dataInputArea);

    chart = new QChart();
    chart->legend()->hide();
    chart->setObjectName("chart");

    chart->setBackgroundBrush(QColor(Graphics::palette().chart));
    chart->setAcceptHoverEvents(true);
    this->setMouseTracking(true);

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
    chart->setMargins(QMargins(0,0,0,0));
    chart->setBackgroundRoundness(0);

    setGraphColor(QColor(Graphics::palette().chartGridlegLowContrast));
    // Default grid transparency 40%
    setGridTransparencyPercent(40);
    // Create clickable grid alpha dot in top-right corner of plot
    gridAlphaDot = new QGraphicsEllipseItem(0,0, gridAlphaDotRadius*1.6, gridAlphaDotRadius*1.6);
    gridAlphaDot->setZValue(1000);
    gridAlphaDot->setPen(Qt::NoPen);
    gridAlphaDot->setBrush(axisX->gridLineColor());
    if (chart->scene()) chart->scene()->addItem(gridAlphaDot);
    else QTimer::singleShot(0, this, [this](){ if (chart->scene() && gridAlphaDot && !gridAlphaDot->scene()) chart->scene()->addItem(gridAlphaDot); layoutGridAlphaDot(); });
    connect(chart, &QChart::plotAreaChanged, this, [this](const QRectF&){ layoutGridAlphaDot(); });
    layoutGridAlphaDot();
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
    }
    // Force immediate repaint after switching to line series.
    updateAxis();
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
    }
    // Force immediate repaint after switching to line series.
    updateAxis();
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
        series->setUseOpenGL(false);
    }
    // Force immediate repaint after switching to scatter; schedule deferred update
    updateAxis();
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
    }

    if(event->type() == QEvent::GraphicsSceneMouseRelease){
        mousePressed = false;
        QGraphicsSceneMouseEvent *ev = (QGraphicsSceneMouseEvent*) event;
        // If released inside (enlarged) dot area, consume and step transparency
        if (isPointInGridAlphaDot(ev->scenePos())) {
            stepGridTransparency();
            QApplication::restoreOverrideCursor();
            return true;
        }
        // Only treat left-button releases as a 'left click' event to avoid
        // changing signal polarity or editing on right-click.
        if (ev->button() == Qt::LeftButton)
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
    if(!points || index < 0 || index >= seriesList.size()) {
        qWarning() << "widgetChart::updateTrace invalid index or null points" << index;
        return;
    }
    seriesList[index]->replace(*points);
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
    chart->setMargins(QMargins(left, top, right, bottom));
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
    this->shift = percentToInternalShift(shift);
    updateAxis();
}

qreal widgetChart::getShift()
{
    return shift;
}

qreal widgetChart::getShiftPercent() const
{
    return internalShiftToPercent(shift);
}

void widgetChart::enableLocalMouseEvents(EventSelection sel)
{
    eventSel = sel;
    // Install on the scene to receive GraphicsScene* events reliably
    if (chart->scene()) {
        chart->scene()->installEventFilter(this);
    } else {
        // Defer until the scene exists (after QChartView creates it)
        QTimer::singleShot(0, this, [this]() {
            if (chart->scene()) {
                chart->scene()->installEventFilter(this);
            }
        });
    }
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

void widgetChart::setGridTransparencyPercent(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    gridTransparencyPercent = percent;
    applyGridTransparency();
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
    applyGridTransparency(); // re-apply alpha to gridlines after color change
}

void widgetChart::applyGridTransparency()
{
    // Convert percent to alpha 0..255 and apply only to grid line colors of main axes
    const int alpha = static_cast<int>(qRound(gridTransparencyPercent * 2.55));
    QColor gridX = axisX->gridLineColor(); gridX.setAlpha(alpha); axisX->setGridLineColor(gridX);
    QColor gridY = axisY->gridLineColor(); gridY.setAlpha(alpha); axisY->setGridLineColor(gridY);

    // Apply alpha to axis line pens too (left/bottom borders), so border lines fade equally
    QPen penX = axisX->linePen(); penX.setColor(QColor(penX.color().red(), penX.color().green(), penX.color().blue(), alpha)); axisX->setLinePen(penX);
    QPen penY = axisY->linePen(); penY.setColor(QColor(penY.color().red(), penY.color().green(), penY.color().blue(), alpha)); axisY->setLinePen(penY);    

    // Update the indicator dot to reflect current grid color
    if (gridAlphaDot) gridAlphaDot->setBrush(gridX);
    // Trigger a repaint
    chart->update();
}

void widgetChart::layoutGridAlphaDot()
{
    if (!chart || !gridAlphaDot) return;
    const QRectF plot = chart->plotArea();
    if (plot.isEmpty()) return;
    const qreal x = plot.right() - gridAlphaDotMargin - gridAlphaDotRadius*1.8;
    const qreal y = plot.top() + gridAlphaDotMargin;
    const QPointF scenePt = chart->mapToScene(QPointF(x, y));
    gridAlphaDot->setPos(scenePt);
}

void widgetChart::setTraceColor(int index, QColor color){
    if(index < 0 || index >= seriesList.size()) {
        qWarning() << "widgetChart::setTraceColor invalid index" << index;
        return;
    }
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
    axisX_UnitRange->setLineVisible(false); // avoid double-drawing along the top border
    axisX_UnitRange->setGridLineVisible(false); // prevent extra edge grid lines from this helper axis
    chart->addAxis(axisX_UnitRange, Qt::AlignTop);
}

void widgetChart::createVerticalMarkers(){
    //prepare markers on X axis
    axisY_UnitRange = new QValueAxis;
    axisY_UnitRange->setRange(0, 1);
    axisY_UnitRange->setLabelsVisible(false);
    axisY_UnitRange->setTickCount(2);
    axisY_UnitRange->setLineVisible(false); // avoid double-drawing along the right border
    axisY_UnitRange->setGridLineVisible(false); // prevent extra edge grid lines from this helper axis
    chart->addAxis(axisY_UnitRange, Qt::AlignRight);
}

void widgetChart::initBrushes()
{
    MarkerPath_ArrowDownBig = QPainterPath();
    MarkerPath_ArrowDownBig.lineTo(10,20);
    MarkerPath_ArrowDownBig.lineTo(20,0);

    MarkerPath_Tick = QPainterPath(QPoint(0,7));
    MarkerPath_Tick.lineTo(14,7);
    MarkerPath_Tick.lineTo(19,10);
    MarkerPath_Tick.lineTo(14,13);
    MarkerPath_Tick.lineTo(0,13);

    MarkerPath_ArrowDownSmall = QPainterPath(QPointF(5,0));
    MarkerPath_ArrowDownSmall.lineTo(10,10);
    MarkerPath_ArrowDownSmall.lineTo(15,0);

    MarkerPath_ArrowUpSmall = QPainterPath(QPointF(5,20));
    MarkerPath_ArrowUpSmall.lineTo(10,10);
    MarkerPath_ArrowUpSmall.lineTo(15,20);

    MarkerPath_Cross = QPainterPath(QPointF(7,7));
    MarkerPath_Cross.lineTo(13,13);
    MarkerPath_Cross.moveTo(7,13);
    MarkerPath_Cross.lineTo(13,7);

    MarkerPath_Circle = QPainterPath(QPointF(10,10));
    MarkerPath_Circle.arcTo(QRectF(8,8,4,4),0,360*16);

    MarkerPath_Trigger = QPainterPath(QPointF(0,5));
    MarkerPath_Trigger.lineTo(15,5);
    MarkerPath_Trigger.lineTo(20,10);
    MarkerPath_Trigger.lineTo(15,15);
    MarkerPath_Trigger.lineTo(13,15);
    MarkerPath_Trigger.lineTo(13,10);
    MarkerPath_Trigger.lineTo(16,10);
    MarkerPath_Trigger.lineTo(16,7);
    MarkerPath_Trigger.lineTo(7,7);
    MarkerPath_Trigger.lineTo(7,10);
    MarkerPath_Trigger.lineTo(10,10);
    MarkerPath_Trigger.lineTo(10,15);
    MarkerPath_Trigger.lineTo(0,15);

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
    painter.drawPath(MarkerPath_ArrowDownBig);
        break;
    case MarkerType::ARROW_UP_SMALL:
    painter.drawPath(MarkerPath_ArrowUpSmall);
        break;
    case MarkerType::ARROW_DOWN_SMALL:
    painter.drawPath(MarkerPath_ArrowDownSmall);
        break;
    case MarkerType::TICK:
    painter.drawPath(MarkerPath_Tick);
        break;
    case MarkerType::CROSS:
        painter.setPen(QPen(QBrush(QColor(Graphics::getChannelColor(channelIndex))), 2.0));
    painter.drawPath(MarkerPath_Cross);
        break;
    case MarkerType::CIRCLE:
    painter.drawPath(MarkerPath_Circle);
        break;
    case MarkerType::TRIGGER:
    painter.drawPath(MarkerPath_Trigger);
        break;
    }
    return marker;
}

void widgetChart::setHorizontalMarker(int channelIndex, qreal value, MarkerType type){
    if(markerHorizontalIndex < markersHorizontal.size()){
        const QPointF pt(HORIZONTAL_MARKER_X_OFFSET,value);
        QVector<QPointF> pts{pt};
        markersHorizontal[markerHorizontalIndex]->setBrush(getBrush(channelIndex,type));
        markersHorizontal[markerHorizontalIndex]->replace(pts);
        ++markerHorizontalIndex;
    }
}

void widgetChart::setVerticalMarker(int channelIndex, qreal value){
    if(markerVerticalIndex < markersVertical.size()){
        const QPointF pt(value,VERTICAL_MARKER_Y_OFFSET);
        triggerShift = value;
        QVector<QPointF> pts{pt};
        markersVertical[markerVerticalIndex]->setBrush(getBrush(channelIndex,MarkerType::ARROW_DOWN_BIG));
        markersVertical[markerVerticalIndex]->replace(pts);
        ++markerVerticalIndex;
    }
}

void widgetChart::setHorizontalCursor(int channelIndex, qreal value, Cursor type)
{
    const QVector<QPointF> pts{QPointF(value,0.0), QPointF(value,1.0)};
    QPen pen(Graphics::getChannelColor(channelIndex));
    pen.setWidth(2);
    if(type == Cursor::CURSOR_A){
        pen.setStyle(Qt::DashLine);
        cursorsHorizontal[0]->setPen(pen);
        cursorsHorizontal[0]->replace(pts);
    }else{
        pen.setStyle(Qt::DashDotLine);
        cursorsHorizontal[1]->setPen(pen);
        cursorsHorizontal[1]->replace(pts);
    }
}

void widgetChart::setVerticalCursor(int channelIndex, qreal value, Cursor type)
{
    const QVector<QPointF> pts{QPointF(0.0,value), QPointF(1.0,value)};
    QPen pen(Graphics::getChannelColor(channelIndex));
    pen.setWidth(2);
    if(type == Cursor::CURSOR_A){
        pen.setStyle(Qt::DashLine);
        cursorsVertical[0]->setPen(pen);
        cursorsVertical[0]->replace(pts);
    }else{
        pen.setStyle(Qt::DashDotLine);
        cursorsVertical[1]->setPen(pen);
        cursorsVertical[1]->replace(pts);
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
    // If style selection is disabled for this chart, show a placeholder action
    if (!allowStyleSelection) {
        // Lazily create placeholder action so translations can be applied centrally
        if (!placeholderAction) {
            placeholderAction = new QAction(tr("Options..."), this);
            placeholderAction->setEnabled(false); // currently just a placeholder
        }
        QMenu tmpMenu(this);
        tmpMenu.addAction(placeholderAction);
        tmpMenu.exec(mapToGlobal(mousePos));
        return;
    }
    menu->exec(mapToGlobal(mousePos));
}

void widgetChart::setAllowStyleSelection(bool allow)
{
    allowStyleSelection = allow;
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

bool widgetChart::mapSceneToPlotNormalized(const QPointF &scenePos, qreal &nx, qreal &ny) const
{
    if (!chart) return false;
    // Convert scene position to chart-local coordinates, then normalize against plotArea
    const QPointF chartPos = chart->mapFromScene(scenePos);
    const QRectF plot = chart->plotArea();
    const qreal w = plot.width();
    const qreal h = plot.height();
    if (w <= 0 || h <= 0) { nx = ny = 0; return false; }
    nx = (chartPos.x() - plot.left()) / w;
    ny = (chartPos.y() - plot.top()) / h;
    // Clamp to 0..1 range
    bool inside = (nx >= 0.0 && nx <= 1.0 && ny >= 0.0 && ny <= 1.0);
    if (nx < 0.0) nx = 0.0; else if (nx > 1.0) nx = 1.0;
    if (ny < 0.0) ny = 0.0; else if (ny > 1.0) ny = 1.0;
    return inside;
}

QChart* widgetChart::getChart() const 
{ 
    return chart;
}

bool widgetChart::isPointInGridAlphaDot(const QPointF &scenePos) const
{
    if (!gridAlphaDot) return false;
    // Center of the visual ellipse in scene coordinates
    QPointF center = gridAlphaDot->sceneBoundingRect().center();
    const qreal visualRadius = (gridAlphaDot->rect().width() * gridAlphaDot->sceneTransform().m11()) / 2.0; // approximate scale (uniform expected)
    const qreal effectiveRadius = visualRadius * gridAlphaDotClickableFactor; // enlarge clickable area
    const qreal dist2 = QLineF(center, scenePos).length();
    return dist2 <= effectiveRadius;
}
