#include "widgetdisplay.h"
#include "ui_widgetdisplay.h"

WidgetDisplay::WidgetDisplay(QString name, QString firstLabelText, QString &unitsStyleSheet,
                             bool showPrgrssBar, int historyTracesNum, int historySize,
                             QWidget *parent) :
    QWidget(parent), ui(new Ui::WidgetDisplay), historySize(historySize), name(name)
{
    ui->setupUi(this);

    loc = QLocale(QLocale::English);

    palette = ui->lcdNumber_avg->palette();

    labelList.append(ui->label_0);
    labelList.append(ui->label_1);
    labelList.append(ui->label_2);
    labelList.append(ui->label_3);
    labelList.append(ui->label_4);

    foreach(QLabel* label, labelList)
        label->hide();

    ui->label_0->setText(firstLabelText);
    ui->label_0->show();

    setUnitsStyle(unitsStyleSheet);
    showBarDisplay(showPrgrssBar);
    displayString("");
    displayAvgString("");
    hideHistoryChartArea();
    historyData = new QVector<QVector<QPointF>>(historyTracesNum);
    createHistoryChart(historyTracesNum);
    createHistoryList();
    setHistorySize(historySize);
    configureCustomDial();
    configureFloatingHistoryNumber();

    QString style;

    style = "QObject{background-color:"+Graphics::COLOR_DISPLAY+";}";
    setStyleSheet(style);

    style = "QObject{background-color:"+Graphics::COLOR_WINDOW_WIDGET+";}";
    ui->line->setStyleSheet(style);

    style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_history_off.png);}"
            "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
    ui->pushButton_history->setStyleSheet(style);

    style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_list.png);}"
            "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
    ui->pushButton_list->setStyleSheet(style);

    style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_clear_history.png);}"
            "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
    ui->pushButton_clear->setStyleSheet(style);

    style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_save.png);}"
            "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
    ui->pushButton_save->setStyleSheet(style);

    connect(ui->pushButton_history, SIGNAL(clicked()),
            this, SLOT(historyButtonClickedCallback()));
    connect(ui->pushButton_clear, SIGNAL(clicked()),
            this, SLOT(clearHistoryButtonClickedCallback()));
    connect(ui->pushButton_list, SIGNAL(clicked()),
            this, SLOT(listChartSwitchClickedCallback()));
    connect(ui->pushButton_save, SIGNAL(clicked()),
            this, SLOT(saveListClickedCallback()));

    connect(ui->dial, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(dialShowMenuOnRightClickCallback(const QPoint &)));
    connect(ui->dial, SIGNAL(valueChanged(int)),
            this, SLOT(dialHistoryValueChangedCallback(int)));
}

WidgetDisplay::~WidgetDisplay()
{
    delete ui;
}

QString WidgetDisplay::formatNumber(double val, char f, int prec){
    return loc.toString(val, f, prec).replace(loc.decimalPoint(), '.');
}

void WidgetDisplay::hideHistoryChartArea(){
    QList<int> sizes = {0, ui->splitter->width()};
    ui->splitter->setSizes(sizes);
}

void WidgetDisplay::hideHistoryPanel(){
    ui->verticalWidget_menu->hide();
}

void WidgetDisplay::setUnitsStyle(QString &unitsStyleSheet){
    ui->styleUnits->setStyleSheet(unitsStyleSheet);
}

void WidgetDisplay::setAvgStyle(QString &avgStyleSheet){
    ui->styleAvg->setStyleSheet(avgStyleSheet);
}

void WidgetDisplay::setErrStyle(QString &errStyleSheet){
    ui->styleErr->setStyleSheet(errStyleSheet);
}

void WidgetDisplay::setQerrStyle(QString &qerrStyleSheet){
    ui->styleQerr->setStyleSheet(qerrStyleSheet);
}

void WidgetDisplay::setTerrStyle(QString &terrStyleSheet){
    ui->styleTerr->setStyleSheet(terrStyleSheet);
}

void WidgetDisplay::setBarStyle(QString &barStyleSheet){
    ui->progressBar->setStyleSheet(barStyleSheet);
}

void WidgetDisplay::showQerrStyle(bool visible){
    (visible) ? ui->styleQerr->show() : ui->styleQerr->hide();
}

void WidgetDisplay::showTerrStyle(bool visible){
    (visible) ? ui->styleTerr->show() : ui->styleTerr->hide();
}

void WidgetDisplay::showQerrTerrStyle(bool visible){
    showQerrStyle(visible);
    showTerrStyle(visible);
}

void WidgetDisplay::showAvgDisplay(bool visible){
    (visible) ? ui->horizontalWidget_avg->show() : ui->horizontalWidget_avg->hide();
}

void WidgetDisplay::showErrDisplay(bool visible){
    (visible) ? ui->horizontalWidget_err->show() : ui->horizontalWidget_err->hide();
}

void WidgetDisplay::showBarDisplay(bool visible){
    (visible) ? ui->horizontalWidget_bar->show() : ui->horizontalWidget_bar->hide();
}

void WidgetDisplay::displayNumber(double number){
    ui->lcdNumber->display(number);
}

void WidgetDisplay::displayString(const QString &string){
    ui->lcdNumber->display(string);
}

void WidgetDisplay::displayAvgNumber(double number){
    ui->lcdNumber_avg->display(number);
}

void WidgetDisplay::displayAvgString(const QString &string){
    ui->lcdNumber_avg->display(string);
}

void WidgetDisplay::displayQerrNumber(double number){
    ui->lcdNumber_qerr->display(number);
}

void WidgetDisplay::displayQerrString(const QString &string){
    ui->lcdNumber_qerr->display(string);
}

void WidgetDisplay::displayTerrNumber(double number){
    ui->lcdNumber_terr->display(number);
}

void WidgetDisplay::displayTerrString(const QString &string){
    ui->lcdNumber_terr->display(string);
}

void WidgetDisplay::changeAvgColor(QColor color){
    palette.setColor(QPalette::WindowText, color);
    ui->lcdNumber_avg->setPalette(palette);
}

void WidgetDisplay::showProgressBar(bool visible){
    (visible) ? ui->progressBar->show() : ui->progressBar->hide();
}

void WidgetDisplay::setProgressBarRange(int min, int max){
    ui->progressBar->setRange(min, max);
}

void WidgetDisplay::setProgressBarColor(QString color){
    if(Graphics::STYLE_TRANSPARENCY_USED)
        color = color.remove("#");
    ui->progressBar->setStyleSheet(QString(Graphics::STYLE_PROGRESS_BAR).arg(color));
    ui->progressBar->repaint();
}

void WidgetDisplay::updateProgressBar(int value){
    ui->progressBar->setValue(value);
    ui->progressBar->repaint();
}

void WidgetDisplay::configLabel(int labelNumber, QString text, QString colorStyle, bool isVisible){
    labelList.at(labelNumber)->setText(text);
    labelList.at(labelNumber)->setStyleSheet("color:" + colorStyle);
    isVisible ? labelList.at(labelNumber)->show() : labelList.at(labelNumber)->hide();
}

QString WidgetDisplay::getLabelText(int labelNumber){
    return labelList.at(labelNumber)->text();
}

void WidgetDisplay::setLabelText(int labelNumber, const QString text){
    labelList.at(labelNumber)->setText(text);
    labelList.at(labelNumber)->show();
}

void WidgetDisplay::setLabelColor(int labelNumber, const QString &textStyleSheet){
    labelList.at(labelNumber)->setStyleSheet(textStyleSheet);
}

void WidgetDisplay::hideLabel(int labelNumber){
    labelList.at(labelNumber)->hide();
}

void WidgetDisplay::showLabel(int labelNumber){
    labelList.at(labelNumber)->show();
}

void WidgetDisplay::drawIndicationFlag(int labelNumber)
{
    if(drawFlag == 0){
        labelList.at(labelNumber)->setText(cFull+cTran1+cTran2+cBlank+cBlank);
        drawFlag++;
    }else if (drawFlag == 1) {
        labelList.at(labelNumber)->setText(cTran1+cFull+cBlank+cBlank+cBlank);
        drawFlag++;
    }else if (drawFlag == 2) {
        labelList.at(labelNumber)->setText(cTran2+cTran1+cFull+cBlank+cBlank);
        drawFlag++;
    }else if (drawFlag == 3) {
        labelList.at(labelNumber)->setText(cBlank+cTran2+cTran1+cFull+cBlank);
        drawFlag++;
    }else if (drawFlag == 4) {
        labelList.at(labelNumber)->setText(cBlank+cBlank+cTran2+cTran1+cFull);
        drawFlag++;
    }else if (drawFlag == 5) {
        labelList.at(labelNumber)->setText(cBlank+cBlank+cBlank+cFull+cTran1);
        drawFlag++;
    }else if (drawFlag == 6) {
        labelList.at(labelNumber)->setText(cBlank+cBlank+cFull+cTran1+cTran2);
        drawFlag++;
    }else if (drawFlag == 7) {
        labelList.at(labelNumber)->setText(cBlank+cFull+cTran1+cTran2+cBlank);
        drawFlag = 0;
    }
}

void WidgetDisplay::clearIndicationFlag(int labelNumber)
{
    labelList.at(labelNumber)->setText(cBlank+cBlank+cBlank+cBlank+cBlank);
}

void WidgetDisplay::setIndicationFlagColor(QString color)
{
    QString cFull = "#FF"+color.remove("#");
    QString cTran1 = "#AA"+color.remove("#");
    QString cTran2 = "#55"+color.remove("#");
    QString cBlank = "#00"+color.remove("#");
    this->cFull = QString("<span style=background-color:"+cFull+";color:"+cBlank+";>--</span>");
    this->cTran1 = QString("<span style=background-color:"+cTran1+";color:"+cBlank+";>--</span>");
    this->cTran2 = QString("<span style=background-color:"+cTran2+";color:"+cBlank+";>--</span>");
    this->cBlank = QString("<span style=background-color:"+cBlank+";color:"+cBlank+";>--</span>");
}

/******************************* HISTORY *********************************/

void WidgetDisplay::configureCustomDial(){
    ui->dial->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->dial->setObjectName("dialHistory");
    ui->dial->drawMarker(false);
    ui->dial->setRange(100, 1000);
    ui->dial->setPageStep(100);
    ui->dial->setSingleStep(20);
    ui->dial->setCustomGraphics(Graphics::STYLE_CUSTOM_DIALS_USED);
}

void WidgetDisplay::configureFloatingHistoryNumber(){
    ui->dial->setEnabled(true);
    ui->dial->setToolTip(QString::number(historySize));
    labelFloatHistNum = new QLabel(this);
    labelFloatHistNum->setGeometry(ui->dial->x()+10, ui->dial->y()+64, 30, 20);
    labelFloatHistNum->setStyleSheet("QLabel{background-color:"+Graphics::COLOR_WINDOW_APP+";}");
    labelFloatHistNum->hide();
    labelFloatHistNum->setNum(100);
}

void WidgetDisplay::createHistoryChart(int historyTracesNum){
    chart = new widgetChart(ui->verticalWidget_history, historyTracesNum);
    chart->setLabelsSize(7);
    chart->formatLabels("%.1f", "%.2g");
    // chart->setGridLinesVisible(true, true);
    chart->setGridDensity(5, 5);
    chart->setLabelsVisible(true, true);
    chart->setGraphColor(QColor(Graphics::COLOR_CHART_GRIDLEG_LOW_CONTRAST));
    chart->setMargins(-12, -5, -6, -10);
    chart->setRangeY(0, 3.3);
    chart->setDataMinMax(0, 10);
    ui->horizontalWidget_graph_2->addWidget(chart);
}

void WidgetDisplay::createHistoryList(){
    list = new WidgetList(ui->verticalWidget_history);
    list->hide();
    ui->horizontalWidget_graph_2->addWidget(list);
}

void WidgetDisplay::setHistorySize(int smplNumber){
    historySize = smplNumber;
}

void WidgetDisplay::clearHistoryChart(){
    chart->clearAll();
}

void WidgetDisplay::clearHistoryList(){
    list->clear();
}

void WidgetDisplay::clearHistoryData(){
    for (int i = 0; i < historyData->length(); i++)
        historyData[0][i].clear();
}

void WidgetDisplay::clearExpiredPointsFromChart(){
    for(int i = 0; i < historyData->length(); i++)
        chart->clearPoint(i, 0);
}

void WidgetDisplay::clearExpiredPointsFromList(){
    list->clearLast();
}

void WidgetDisplay::clearExpiredData(){
    for(int i = 0; i < historyData->length(); i++)
        historyData[0][i].removeFirst();
}

void WidgetDisplay::appendNewHistorySample(QString prefix, double sample, QString affix, float timeStep){
    timeAxisMax += timeStep;
    historyData[0][0].reserve(historyData[0].length());
    historyData[0][0].append(QPointF(timeAxisMax, sample));

    if(sample > rememberMax){
        setHistoryMinMaxData(0, sample+(sample/10));
        rememberMax = sample;
    }

    if(historyData[0].at(0).length() > historySize){
        timeAxisMin += timeStep;
        clearExpiredData();
        clearExpiredPointsFromChart();
        clearExpiredPointsFromList();
    }

    setHistoryMinMaxTime(timeAxisMin, timeAxisMax);
    chart->appendToTrace(0, &historyData[0][0]);
    list->appendNumber(timeAxisMax, prefix, sample, affix);
}

/* Brief: Appends a sample to the selected trace and associates it
 * to the list next to the one added by appendNewHistorySample()
 * traceIndex: ...
 * prefix, affix: Strings added in front of and behind the sample in the list
 * sample: sample to append
 */
void WidgetDisplay::associateSample(int traceIndex, QString prefix, double sample, QString affix){
    historyData[0][traceIndex].append(QPointF(timeAxisMax, sample));
    chart->appendToTrace(traceIndex, &historyData[0][traceIndex]);
    list->associateNumber(prefix, sample, affix);
}

void WidgetDisplay::setHistoryMinMaxTime(qreal minX, qreal maxX){
    chart->setRangeX(minX, maxX);
}

void WidgetDisplay::setHistoryMinMaxData(qreal minY, qreal maxY){
    chart->setRangeY(minY, maxY);
}

void WidgetDisplay::paintEvent(QPaintEvent *event)
{
    int width = ui->lcdNumber->size().width()/9;
    if(abs(ui->styleUnits->size().width()-width)>5)
        ui->styleUnits->setMaximumSize(QSize(width,width));
    QWidget::paintEvent(event);
}

void WidgetDisplay::updateHistoryData(QVector<QPointF> *points, int index){    
    chart->updateTrace(points, index);
}

void WidgetDisplay::setTraceColor(QString color, int index)
{
    chart->setTraceColor(index,QColor(color));
}

void WidgetDisplay::recalcHistorySizeAndSetDial(int reqSize)
{
    ui->dial->setValue(reqSize);

    int actSize = historyData[0][0].length();

    if(actSize > reqSize){
        int difference = actSize - reqSize;
        for(int i = 0; i < difference; i++){
            clearExpiredData();
            clearExpiredPointsFromList();
            clearExpiredPointsFromChart();
        }
        timeAxisMin = historyData[0][0].first().x();
        setHistoryMinMaxTime(timeAxisMin, timeAxisMax);
    }
}

/*************************************** Callbacks *****************************************/

void WidgetDisplay::clearHistoryButtonClickedCallback(){
    clearHistoryChart();
    clearHistoryList();
    clearHistoryData();
    timeAxisMax = 0;
    timeAxisMin = 0;
}

void WidgetDisplay::historyButtonClickedCallback(){
    QString style;
    QList<int> sizes = ui->splitter->sizes();
    int cmpltWidth = ui->splitter->width();

    if(historyView == DISABLED){
        sizes = {cmpltWidth / 3, cmpltWidth / 3 * 2};
        style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_history_on.png);}"
                "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
        historyView = ENABLED;
    }else {
        sizes = {0, cmpltWidth};
        style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_history_off.png);}"
                "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
        historyView = DISABLED;
    }

    ui->splitter->setSizes(sizes);
    ui->pushButton_history->setStyleSheet(style);
}

void WidgetDisplay::listChartSwitchClickedCallback(){
    QString style;
    if(listView == DISABLED){
        style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_chart.png);}"
                "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
        chart->hide();
        list->show();
        listView = ENABLED;
    }else {
        style = "QPushButton{image: url("+Graphics::getGraphicsPath()+"icon_list.png);}"
                "QPushButton:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}";
        chart->show();
        list->hide();
        listView = DISABLED;
    }

    ui->pushButton_list->setStyleSheet(style);
}

void WidgetDisplay::saveListClickedCallback(){
    list->saveList(name);
}

void WidgetDisplay::dialHistoryValueChangedCallback(int val){
    recalcHistorySizeAndSetDial(historySize = val);
    QString str = QString::number(val);
    labelFloatHistNum->setText(str);
    ui->dial->setToolTip(str);

    labelFloatHistNum->show();
    Timing *timer = new Timing();
    timer->sleep(450);
    labelFloatHistNum->hide();
}

void WidgetDisplay::dialShowMenuOnRightClickCallback(const QPoint &mousePos){
    QMenu menu(tr("Context menu"), this);

    QAction s100("100", this);
    QAction s300("300", this);
    QAction s500("500", this);
    QAction s700("700", this);
    QAction s1000("1000", this);

    connect(&s100, SIGNAL(triggered()), this, SLOT(changeHistorySizeTo100()));
    connect(&s300, SIGNAL(triggered()), this, SLOT(changeHistorySizeTo300()));
    connect(&s500, SIGNAL(triggered()), this, SLOT(changeHistorySizeTo500()));
    connect(&s700, SIGNAL(triggered()), this, SLOT(changeHistorySizeTo700()));
    connect(&s1000, SIGNAL(triggered()), this, SLOT(changeHistorySizeTo1000()));

    menu.addAction(&s100);
    menu.addAction(&s300);
    menu.addAction(&s500);
    menu.addAction(&s700);
    menu.addAction(&s1000);

    menu.exec(mapToGlobal(mousePos));
}

void WidgetDisplay::changeHistorySizeTo100(){
    recalcHistorySizeAndSetDial(historySize = 100);
}

void WidgetDisplay::changeHistorySizeTo300(){
    recalcHistorySizeAndSetDial(historySize = 300);
}

void WidgetDisplay::changeHistorySizeTo500(){
    recalcHistorySizeAndSetDial(historySize = 500);
}

void WidgetDisplay::changeHistorySizeTo700(){
    recalcHistorySizeAndSetDial(historySize = 700);
}

void WidgetDisplay::changeHistorySizeTo1000(){
    recalcHistorySizeAndSetDial(historySize = 1000);
}
