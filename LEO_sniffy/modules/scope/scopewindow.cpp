/*
Window for scope
left - area for charts
right - area for setting filled with different widgets

Comment: This will be similar for all features but making this window abstract made me
crazy so I keept it like this and we will have to specify it for all features like gen, counter etc.
But it can be easily coppied so we can keep some template file.
*/
#include "scopewindow.h"
#include "ui_scopewindow.h"

ScopeWindow::ScopeWindow(ScopeConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScopeWindow),
    config(config)
{
    ui->setupUi(this);

    chart = new widgetChart(ui->widget_chart, 5);
    chart->setRange(-0.1, 0.1, CHART_MIN_Y, CHART_MAX_Y);
    chart->enableLocalMouseZoom();
    chart->setGraphColor(QColor(COLOR_GREY));

    chartFFT = new widgetChart(ui->widget_chart, 5);
    chartFFT->setRange(0, 50000, -100, 100);
    chartFFT->setDataMinMax(0,50000);
    chartFFT->enableLocalMouseZoom();
    chartFFT->setGraphColor(QColor(COLOR_GREY));

    ui->verticalLayout_chart->addWidget(chartFFT);
    ui->verticalLayout_chart->addWidget(chart);
    chartFFT->hide();

    labelInfoPanel = new WidgetLabelArea(ui->widget_info);
    ui->verticalLayout_info->addWidget(labelInfoPanel);

    // ********************* insert top options *********************
    widgetTab *tabs = new widgetTab(ui->widget_settings,5);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("Set",0);
    tabs->setText("Meas",1);
    tabs->setText("Cursors",2);
    tabs->setText("Math",3);
    tabs->setText("Adv",4);

    // ************************* creating widget general settings *******************
    panelSet = new PanelSettings(tabs->getLayout(0),tabs);
    panelSet->setObjectName("panelSet");

    connect(panelSet->dialTimeBase,SIGNAL(valueChanged(float)),this,SLOT(timeBaseCallback(float)));
    connect(panelSet->dialPretrigger,SIGNAL(valueChanged(float)),this,SLOT(pretriggerCallback(float)));
    connect(panelSet->buttonsChannelEnable,SIGNAL(statusChanged(int)),this,SLOT(channelEnableCallback(int)));
    connect(panelSet->buttonsTriggerMode,SIGNAL(clicked(int)),this,SLOT(triggerModeCallback(int)));
    connect(panelSet->buttonsTriggerEdge,SIGNAL(clicked(int)),this,SLOT(triggerEdgeCallback(int)));
    connect(panelSet->buttonsTriggerChannel,SIGNAL(clicked(int)),this,SLOT(triggerChannelCallback(int)));
    connect(panelSet->dialTriggerValue,SIGNAL(valueChanged(float)),this,SLOT(triggerValueCallback(float)));
    connect(panelSet->buttonsMemorySet,&WidgetButtons::clicked,this,&ScopeWindow::longMemoryCallback);

    connect(panelSet->buttonsChannelVertical,&WidgetButtons::clicked,this, &ScopeWindow::channelVerticalCallback);
    connect(panelSet->dialVerticalScale,&WidgetDial::valueChanged,this,&ScopeWindow::channelVerticalScaleCallback);
    connect(panelSet->dialVerticalShift,&WidgetDialRange::valueChanged,this,&ScopeWindow::channelVerticalShiftCallback);

    // ************************* creating widget measurement *******************
    panelMeas = new PanelMeasurement(tabs->getLayout(1),tabs);

    connect(panelMeas, &PanelMeasurement::measurementAdded, this,&ScopeWindow::measurementAddedCallback);
    connect(panelMeas, &PanelMeasurement::measurementClearAll, this, &ScopeWindow::measurementClearCallback);

    // ********************* create panel Cursors ****************
    panelCursors = new PanelCursors(tabs->getLayout(2),tabs);
    connect(panelCursors->cursorTypeButtons,&WidgetButtons::clicked,this,&ScopeWindow::cursorTypeCallback);
    connect(panelCursors->channelButtons,&WidgetButtons::clicked,this,&ScopeWindow::cursorChannelCallback);
    connect(panelCursors->cursorHorADial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueHorACallback);
    connect(panelCursors->cursorHorBDial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueHorBCallback);
    connect(panelCursors->cursorVerADial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueVerACallback);
    connect(panelCursors->cursorVerBDial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueVerBCallback);

    // ********************* create panel Math ****************
    panelMath = new PanelMath(tabs->getLayout(3),tabs);
    connect(panelMath,&PanelMath::expressionChanged,this,&ScopeWindow::mathExpressionCallback);
    connect(panelMath,&PanelMath::fftChanged,this,&ScopeWindow::fftChangedCallback);

    // ********************* create panel Advanced ****************
    panelAdvanced = new PanelAdvanced(tabs->getLayout(4),tabs);
    connect(panelAdvanced->resolutionButtons,&WidgetButtons::clicked,this,&ScopeWindow::resolutionChangedCallback);
    connect(panelAdvanced->samplingFrequencyInput,&WidgetTextInput::numberChanged,this,&ScopeWindow::samplingFreqInputCallback);
    connect(panelAdvanced->dataLengthInput,&WidgetTextInput::numberChanged,this,&ScopeWindow::longMemoryCallback);

    //connect top slider and chart and other stuff
    connect(ui->sliderSignal, &QSlider::valueChanged, this, &ScopeWindow::sliderShiftCallback);
    connect(chart,&widgetChart::localZoomChanged,this,&ScopeWindow::chartLocalZoomCallback);
}

ScopeWindow::~ScopeWindow()
{
    delete ui;
}

void ScopeWindow::paintEvent(QPaintEvent *event){
    int handleW = ui->sliderSignal->size().width()/chart->getZoom()/chart->getLocalZoom();
    ui->sliderSignal->setStyleSheet("QSlider::groove:horizontal {background: url("+Colors::getGraphicsPath()+"signalBackground.png) center;"
                                        "background-color: "+Colors::getDataAreaColor()+";border: 1px solid #777;margin-top: 3px;margin-bottom: 3px;}"
                                                                                         "QSlider::handle:horizontal {background: rgba(0, 0, 0, 150);border: 2px solid #777;margin-top: -3px;"
                                                                                         "margin-bottom: -3px;border-radius: 4px;width:"+QString::number(handleW)+QString::fromUtf8("px;}"));
    event->accept();
}


void ScopeWindow::showDataTraces(QVector<QVector<QPointF>> dataSeries, float timeBase, int triggerChannelIndex){
    updateChartTimeScale(timeBase);

    this->triggerChannelIndex = triggerChannelIndex;
    ChartData = dataSeries;
    paintTraces(ChartData,ChartMathData);
}

void ScopeWindow::paintTraces(QVector<QVector<QPointF>> dataSeries, QVector<QPointF> mathSeries){
    chart->clearAll();
    labelInfoPanel->setTriggerLabelText("");
    labelInfoPanel->hideChannelLabels();

    //paint math
    paintMath(mathSeries);

    //paint data traces
    for (int i = 0; i < dataSeries.length(); i++){
        if(panelSet->buttonsChannelEnable->isChecked(i)){

            //put channel in scale
            for (int j = 0; j < dataSeries[i].length(); j++){
                dataSeries[i][j].setY((dataSeries[i][j].y()+config->channelOffset[i])/config->channelScale[i]);
            }

            chart->updateTrace(&dataSeries[i], i);

            float zeroMarkerPosition = config->channelOffset[i]/config->channelScale[i];
            if(zeroMarkerPosition>=CHART_MAX_Y){
                chart-> setHorizontalMarker(i,CHART_MAX_Y,MarkerType::ARROW_UP_SMALL);
            }else if(zeroMarkerPosition<=CHART_MIN_Y){
                chart-> setHorizontalMarker(i,CHART_MIN_Y,MarkerType::ARROW_DOWN_SMALL);
            }else{
                chart-> setHorizontalMarker(i,zeroMarkerPosition,MarkerType::TICK);
            }

            float triggerMarkerPosition = (config->channelOffset[i]+(qreal(config->triggerLevel)/65535*(config->rangeMax-config->rangeMin)/1000))/config->channelScale[i];
            if(panelSet->buttonsTriggerChannel->getSelectedIndex()==i){
                chart-> setHorizontalMarker(i,triggerMarkerPosition,MarkerType::TRIGGER);
            }

            labelInfoPanel->setChannelLabelVisible(i,true);
            labelInfoPanel->setChannelScale(i,LabelFormator::formatOutout(config->channelScale[i],"V/div"));

            panelCursors->cursorHorADial->updateRange(config->timeMin,config->timeMax);
            panelCursors->cursorHorBDial->updateRange(config->timeMin,config->timeMax);
            panelCursors->cursorVerADial->updateRange(config->rangeMin,(float)(config->rangeMax)/1000);
            panelCursors->cursorVerBDial->updateRange(config->rangeMin,(float)(config->rangeMax)/1000);
        }
    }
    chart->setVerticalMarker(triggerChannelIndex,0);
}

void ScopeWindow::paintMath(QVector<QPointF> mathSeries){
    if(mathSeries.length()<=1){
        return;
    }
    for (int k = 0; k < ChartMathData.length(); k++){
        mathSeries[k].setY((mathSeries[k].y()+config->channelOffset[4])/config->channelScale[4]);
    }
    chart->updateTrace(&mathSeries, 4);
    float zeroMarkerPosition = config->channelOffset[4]/config->channelScale[4];
    if(zeroMarkerPosition>=CHART_MAX_Y){
        chart-> setHorizontalMarker(4,CHART_MAX_Y,MarkerType::ARROW_UP_SMALL);
    }else if(zeroMarkerPosition<=CHART_MIN_Y){
        chart-> setHorizontalMarker(4,CHART_MIN_Y,MarkerType::ARROW_DOWN_SMALL);
    }else{
        chart-> setHorizontalMarker(4,zeroMarkerPosition,MarkerType::TICK);
    }

    labelInfoPanel->setChannelLabelVisible(4,true);
    labelInfoPanel->setChannelScale(4,LabelFormator::formatOutout(config->channelScale[4],"V/div"));
}

void ScopeWindow::setDataMinMaxTimeAndZoom(qreal minX, qreal maxX, qreal zoom){
    chart->setDataMinMax(minX,maxX);
    chart->setZoom(zoom*1.2);
}

void ScopeWindow::channelVerticalCallback(int index){
    config->selectedChannelIndexVertical = index;
    panelSet->dialVerticalScale->setColor(Colors::getChannelColor(index));
    if(config->channelScaleIndex[index]==-1){
        config->channelScaleIndex[index] = panelSet->dialVerticalScale->getDefaultIndex();
        config->channelOffsetIndex[index] = panelSet->dialVerticalShift->getDefaultRealValue();
    }
    panelSet->dialVerticalScale->setSelectedIndex(config->channelScaleIndex[index]);

    panelSet->dialVerticalShift->setColor(Colors::getChannelColor(index));
    panelSet->dialVerticalShift->setRealValue(config->channelOffset[index]);
}

void ScopeWindow::channelVerticalScaleCallback(float value){
    config->channelScale[config->selectedChannelIndexVertical] = value;
    config->channelScaleIndex[config->selectedChannelIndexVertical] = panelSet->dialVerticalScale->getSelectedIndex();
    paintTraces(ChartData,ChartMathData);
}
void ScopeWindow::channelVerticalShiftCallback(float value){
    config->channelOffset[config->selectedChannelIndexVertical] = value;
    paintTraces(ChartData,ChartMathData);
}

void ScopeWindow::timeBaseCallback(float value){
    emit timeBaseChanged(value);
    updateChartTimeScale(value);
    previousTimeBase = value;
}
void ScopeWindow::samplingFreqInputCallback(int freq){
    timeBaseCallback(1.0/(qreal)(freq)*100);
}

void ScopeWindow::dataLengthInputCallback(int length)
{
    if (length>=100) emit memoryLengthChanged(length);
}

void ScopeWindow::longMemoryCallback(int index){
    emit memoryLengthChanged(index);
}

void ScopeWindow::pretriggerCallback(float value){
    emit pretriggerChanged(value);
}

void ScopeWindow::triggerValueCallback(float value){
    emit triggerValueChanged(value/3.3*100);
    paintTraces(ChartData,ChartMathData);
}

void ScopeWindow::triggerChannelCallback(int index){
    emit triggerChannelChanged(index);
    paintTraces(ChartData,ChartMathData);

    panelSet->dialPretrigger->setColor(Colors::getChannelColor(index));
    panelSet->dialTriggerValue->setColor(Colors::getChannelColor(index));
    panelSet->buttonsTriggerEdge->setColor(Colors::getChannelColor(index),0);
    panelSet->buttonsTriggerEdge->setColor(Colors::getChannelColor(index),1);
}

void ScopeWindow::triggerEdgeCallback(int index){
    if(index==0){
        emit triggerEdgeChanged(ScopeTriggerEdge::EDGE_RISING);
    }else if(index==1){
        emit triggerEdgeChanged(ScopeTriggerEdge::EDGE_FALLING);
    }
}

void ScopeWindow::triggerModeCallback(int index){
    if(index==0){
        if(panelSet->buttonsTriggerMode->getText(0)=="Stop"){
            panelSet->buttonsTriggerMode->setColor(QString::fromUtf8(COLOR_ORANGE),0);
            emit triggerModeChanged(ScopeTriggerMode::TRIG_STOP);
            panelSet->buttonsTriggerMode->setText("Single",0);
            labelInfoPanel->setTriggerLabelText("");

        }else if(panelSet->buttonsTriggerMode->getText(0)=="Single"){
            emit triggerModeChanged(ScopeTriggerMode::TRIG_SINGLE);
            panelSet->buttonsTriggerMode->setColor(QString::fromUtf8(COLOR_GREEN),0);
            panelSet->buttonsTriggerMode->setText("Stop",0);
        }
    }else if(index==1){
        emit triggerModeChanged(ScopeTriggerMode::TRIG_NORMAL);
        panelSet->buttonsTriggerMode->setText("Stop",0);
    }else if (index==2){
        emit triggerModeChanged(ScopeTriggerMode::TRIG_AUTO);
        panelSet->buttonsTriggerMode->setText("Stop",0);
    }
}


void ScopeWindow::channelEnableCallback(int buttonStatus){
    panelSet->buttonsTriggerChannel->disableAll();

    //if current trigger channel is disabled then select channel one for trigger
    if(!(panelSet->buttonsTriggerChannel->getStatus() & buttonStatus)){
        panelSet->buttonsTriggerChannel->setChecked(true,0);
        triggerChannelCallback(0);
    }

    if(buttonStatus & 0x01){
        panelSet->buttonsTriggerChannel->setDisabledButton(false,0);
    }
    if(buttonStatus & 0x02){
        panelSet->buttonsTriggerChannel->setDisabledButton(false,1);
    }
    if(buttonStatus & 0x04){
        panelSet->buttonsTriggerChannel->setDisabledButton(false,2);
    }
    if(buttonStatus & 0x08){
        panelSet->buttonsTriggerChannel->setDisabledButton(false,3);
    }
    emit channelEnableChanged(buttonStatus);
}


void ScopeWindow::measurementAddedCallback(Measurement *m){
    emit measurementChanged (m);
}
void ScopeWindow::measurementClearCallback(){
    emit measurementClearChanged ();
}

void ScopeWindow::mathExpressionCallback(QString exp)
{
    emit mathExpressionChanged(exp);
    ChartMathData.clear();
    paintTraces(ChartData,ChartMathData);
}

void ScopeWindow::fftChangedCallback(int length, FFTWindow window, FFTType type, int channelIndex)
{
    emit fftChanged(length,window,type,channelIndex);
    ChartFFTData.clear();
    if(length !=0){
        chartFFT->show();
        chartFFT->clearAll();
        //paintTraces(ChartData,ChartMathData); TODO paint in a additional chart (clear)
    }else{
        chartFFT->hide();
    }
}

void ScopeWindow::sliderShiftCallback(int value){
    chart->setShift((float)value/10);
    config->chartShift = (float)value/10;
}

void ScopeWindow::chartLocalZoomCallback()
{
    if(ui->sliderSignal->value()!= (int)(chart->getShift()*1000)){
        ui->sliderSignal->setValue((chart->getShift()*2000)-1000);
    }
    updateChartTimeScale(config->timeBase);
}

void ScopeWindow::cursorTypeCallback(int index)
{
    chart->clearAllCursors();
    if(index==0){
        config->cursorsActiveIndex = 0;
        panelCursors->channelButtons->setEnabled(false);
        panelCursors->cursorHorADial->hide();
        panelCursors->cursorHorBDial->hide();
        panelCursors->cursorVerADial->hide();
        panelCursors->cursorVerBDial->hide();
    }else if(index==1){ //Horizontal
        config->cursorsActiveIndex = 1;
        panelCursors->channelButtons->setEnabled(true);
        panelCursors->cursorHorADial->show();
        panelCursors->cursorHorBDial->show();
        panelCursors->cursorVerADial->hide();
        panelCursors->cursorVerBDial->hide();
    }else if(index==2){ //Vertical
        config->cursorsActiveIndex = 2;
        panelCursors->channelButtons->setEnabled(true);
        panelCursors->cursorHorADial->hide();
        panelCursors->cursorHorBDial->hide();
        panelCursors->cursorVerADial->show();
        panelCursors->cursorVerBDial->show();
    }
    labelInfoPanel->hideCursorReadings();
    cursorChannelCallback(config->cursorChannelIndex);
    updateCursorReadings();
}

void ScopeWindow::cursorChannelCallback(int index)
{
    config->cursorChannelIndex = index;
    panelCursors->cursorHorADial->setColor(Colors::getChannelColor(index));
    panelCursors->cursorHorBDial->setColor(Colors::getChannelColor(index));
    panelCursors->cursorVerADial->setColor(Colors::getChannelColor(index));
    panelCursors->cursorVerBDial->setColor(Colors::getChannelColor(index));
    if(config->cursorsActiveIndex == 2){
        chart->setVerticalCursor(config->cursorChannelIndex,(panelCursors->cursorVerADial->getRealValue()+config->channelOffset[config->cursorChannelIndex])/config->channelScale[config->cursorChannelIndex],Cursor::CURSOR_A);
        chart->setVerticalCursor(config->cursorChannelIndex,(panelCursors->cursorVerBDial->getRealValue()+config->channelOffset[config->cursorChannelIndex])/config->channelScale[config->cursorChannelIndex],Cursor::CURSOR_B);
    }
    if(config->cursorsActiveIndex == 1){
        chart->setHorizontalCursor(config->cursorChannelIndex,panelCursors->cursorHorADial->getRealValue(),Cursor::CURSOR_A);
        chart->setHorizontalCursor(config->cursorChannelIndex,panelCursors->cursorHorBDial->getRealValue(),Cursor::CURSOR_B);
    }
    labelInfoPanel->setCursorReadingsColor(config->cursorChannelIndex);
}

void ScopeWindow::cursorValueHorACallback(float value)
{
    if(config->cursorsActiveIndex == 1){
        chart->setHorizontalCursor(config->cursorChannelIndex,value,Cursor::CURSOR_A);
        updateCursorReadings();
    }
}

void ScopeWindow::cursorValueHorBCallback(float value)
{
    if(config->cursorsActiveIndex == 1){
        chart->setHorizontalCursor(config->cursorChannelIndex,value,Cursor::CURSOR_B);
        updateCursorReadings();
    }
}

void ScopeWindow::cursorValueVerACallback(float value)
{
    if(config->cursorsActiveIndex == 2){
        chart->setVerticalCursor(config->cursorChannelIndex,(value+config->channelOffset[config->cursorChannelIndex])/config->channelScale[config->cursorChannelIndex],Cursor::CURSOR_A);
        updateCursorReadings();
    }
}

void ScopeWindow::cursorValueVerBCallback(float value)
{
    if(config->cursorsActiveIndex == 2){
        chart->setVerticalCursor(config->cursorChannelIndex,(value+config->channelOffset[config->cursorChannelIndex])/config->channelScale[config->cursorChannelIndex],Cursor::CURSOR_B);
        updateCursorReadings();
    }
}

void ScopeWindow::resolutionChangedCallback(int index){
    if(index==0){
        emit resolutionChanged(8);
    }else{
        emit resolutionChanged(12);
    }
}

void ScopeWindow::updateCursorReadings()
{
    if(config->cursorsActiveIndex == 2){
        qreal curA = panelCursors->cursorVerADial->getRealValue();
        qreal curB = panelCursors->cursorVerBDial->getRealValue();
        labelInfoPanel->setCursorVoltageReadings(curA,curB);
    }
    if(config->cursorsActiveIndex == 1){
        qreal curA = chart->getSignalValue(config->cursorChannelIndex,panelCursors->cursorHorADial->getRealValue());
        qreal curB = chart->getSignalValue(config->cursorChannelIndex,panelCursors->cursorHorBDial->getRealValue());
        curA = (curA * config->channelScale[config->cursorChannelIndex])-config->channelOffset[config->cursorChannelIndex];
        curB = (curB * config->channelScale[config->cursorChannelIndex])-config->channelOffset[config->cursorChannelIndex];

        labelInfoPanel->setCursorVoltageReadings(curA,curB);
        labelInfoPanel->setCursorTimeReadings(panelCursors->cursorHorADial->getRealValue(),panelCursors->cursorHorBDial->getRealValue());
    }
}

void ScopeWindow::updateMeasurement(QList<Measurement*> m){
    labelInfoPanel->setMeasurements(m);
}

void ScopeWindow::updateMath(QVector<QPointF> mathTrace)
{
    ChartMathData = mathTrace;
    paintTraces(ChartData,ChartMathData);
}

void ScopeWindow::updateFFTchart(QVector<QPointF> fftTrace)
{
    ChartFFTData = fftTrace;
    chartFFT->clearAll();
    chartFFT->updateTrace(&ChartFFTData,0);
    //TODO paint in a chart
    qDebug () << "FFT was calculated and received by window" << fftTrace.length();
}

void ScopeWindow::mathError(int errorPosition)
{
    panelMath->symbolicError(errorPosition);
}

void ScopeWindow::restoreGUIAfterStartup()
{
    chart->setLocalZoom(config->chartLocalZoom);
    chart->setShift(config->chartShift);
    ui->sliderSignal->setValue((chart->getShift()*2000)-1000);

    int vertical = panelSet->buttonsChannelVertical->getSelectedIndex();
    panelSet->dialVerticalScale->setColor(Colors::getChannelColor(vertical));
    panelSet->dialVerticalShift->setColor(Colors::getChannelColor(vertical));

    channelEnableCallback(panelSet->buttonsChannelEnable->getStatus());
    cursorTypeCallback(panelCursors->cursorTypeButtons->getSelectedIndex());
    triggerChannelCallback(panelSet->buttonsTriggerChannel->getSelectedIndex());
    resolutionChangedCallback(panelAdvanced->resolutionButtons->getSelectedIndex());

    panelMeas->setMeasButtonsColor(panelMeas->channelButtons->getSelectedIndex());
    panelMath->typeChanged(panelMath->mathType->getSelectedIndex());
    if(panelSet->buttonsTriggerMode->getSelectedIndex()==0){
        panelSet->buttonsTriggerMode->setText("Single");
        triggerModeCallback(0);
    }
}

void ScopeWindow::singleSamplingDone(){
    panelSet->buttonsTriggerMode->setText("Single",0);
    panelSet->buttonsTriggerMode->setColor(COLOR_ORANGE,0);
    labelInfoPanel->setTriggerLabelText("");
}

void ScopeWindow::samplingOngoing(){
    labelInfoPanel->setTriggerLabelText("Sampling");
}

void ScopeWindow::triggerCaptured(){
    labelInfoPanel->setTriggerLabelText("Trig");
}

void ScopeWindow::setRealSamplingRate(int smpl){
    labelInfoPanel->setSamplingRateLabelText(LabelFormator::formatOutout(smpl,"SPS") + "  ("+LabelFormator::formatOutout(config->dataLength,"")+")");
}

void ScopeWindow::updateChartTimeScale(float timeBase){
    if(previousTimeBase !=0 && previousTimeBase != timeBase){
        chart->setZoom(chart->getZoom()*previousTimeBase/timeBase);
    }
    if(chart->getLocalZoom()!=1.0){
        labelInfoPanel->setStyleSheet(QString::fromUtf8("QLabel#label_scale {color:")+COLOR_RED+"}");
    }else{
        labelInfoPanel->setStyleSheet(QString::fromUtf8("color:")+COLOR_WHITE);
    }
    config->chartLocalZoom = chart->getLocalZoom();
    labelInfoPanel->setScaleLabelText(LabelFormator::formatOutout(timeBase/chart->getLocalZoom(),"s/div"));
}


