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

    ui->widget_top -> setStyleSheet("background-color:" + Graphics::palette().dataInputArea);
    ui->widget_left->setContentsMargins(4,4,4,4);

    chart = new widgetChart(ui->widget_chart, 5);
    chart->setRange(-0.1, 0.1, CHART_MIN_Y, CHART_MAX_Y);
    chart->enableLocalMouseEvents(EventSelection::ALL);
    chart->hideAxislabels();
    chart->setMargins(-29,-25,-25,-23);
    chart->setGridDensity(DEFAULT_CHART_DIV+1,9);

    chartFFT = new widgetChart(ui->widget_chart, 5);
    chartFFT->setRange(0, 50000, 0, 8);
    chartFFT->setDataMinMax(0,50000);
    chartFFT->enableLocalMouseEvents(EventSelection::ALL);

    splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(chartFFT);
    splitter->addWidget(chart);

    chartFFT->hide();

    QList<int> sizes = splitter->sizes();
    int hght = splitter->height();
    sizes = {hght, hght};
    splitter->setSizes(sizes);

    ui->verticalLayout_chart->addWidget(splitter);


    labelInfoPanel = new WidgetLabelArea(ui->widget_info);
    ui->widget_info -> setStyleSheet("background-color:" + Graphics::palette().dataInputArea);
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
    connect(panelSet->buttonsMemorySet,&WidgetButtons::clicked,this,&ScopeWindow::memoryPolicyCallback);

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
    connect(panelCursors->cursorHorADial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueHorCallback);
    connect(panelCursors->cursorHorBDial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueHorCallback);
    connect(panelCursors->cursorFFTHorADial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueHorCallback);
    connect(panelCursors->cursorFFTHorBDial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueHorCallback);
    connect(panelCursors->cursorVerADial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueVerCallback);
    connect(panelCursors->cursorVerBDial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueVerCallback);
    connect(panelCursors->cursorFFTVerADial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueVerCallback);
    connect(panelCursors->cursorFFTVerBDial,&WidgetDialRange::valueChanged,this,&ScopeWindow::cursorValueVerCallback);

    // ********************* create panel Math ****************
    panelMath = new PanelMath(tabs->getLayout(3),tabs);
    connect(panelMath,&PanelMath::expressionChanged,this,&ScopeWindow::mathExpressionCallback);
    connect(panelMath,&PanelMath::fftChanged,this,&ScopeWindow::fftChangedCallback);
    connect(panelMath,&PanelMath::fftchartChanged,this,&ScopeWindow::fftchartChangedCallback);
    connect(panelMath,&PanelMath::mathTypeChanged,this,&ScopeWindow::mathTypeChangedCallback);

    // ********************* create panel Advanced ****************
    panelAdvanced = new PanelAdvanced(tabs->getLayout(4),tabs);
    connect(panelAdvanced->resolutionButtons,&WidgetButtons::clicked,this,&ScopeWindow::resolutionChangedCallback);
    connect(panelAdvanced->samplingFrequencyInput,&WidgetTextInput::numberChanged,this,&ScopeWindow::SamplingFreqCustomInputCallback);
    connect(panelAdvanced->dataLengthInput,&WidgetTextInput::numberChanged,this,&ScopeWindow::memoryCustomLengthCallback);

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
    // Build slider stylesheet:
    // - Groove uses themed background + frame.
    // - Handle color uses an 0xBA (~73% alpha) prefix plus the base button color without '#'.
    //   We call QString(Graphics::palette().backgroundButton).remove("#") on a temporary copy so
    //   the cached palette string is never mutated; only the formatted value is used here.
    ui->sliderSignal->setStyleSheet("QSlider::groove:horizontal {background: url("+Graphics::getGraphicsPath()+"signalBackground.png) center;"
                                        "background-color: "+Graphics::palette().windowWidget+";border: 1px solid "+Graphics::palette().textLabel+";margin-top: 3px;margin-bottom: 3px;}"
                                                                                         "QSlider::handle:horizontal {background: #ba"+QString(Graphics::palette().backgroundButton).remove("#") + ";border: 2px solid "+Graphics::palette().textLabel+";margin-top: -3px;"
                                                                                         "margin-bottom: -3px;border-radius: 4px;width:"+QString::number(handleW)+"px;}");
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
            labelInfoPanel->setChannelScale(i,LabelFormator::formatOutout(config->channelScale[i],"V/d"));

            if(config->FFTenabled){
                panelCursors->cursorFFTHorADial->updateRange(0,config->realSamplingRate/2);
                panelCursors->cursorFFTHorBDial->updateRange(0,config->realSamplingRate/2);
            }else{
                panelCursors->cursorHorADial->updateRange(config->timeMin,config->timeMax);
                panelCursors->cursorHorBDial->updateRange(config->timeMin,config->timeMax);
                panelCursors->cursorVerADial->updateRange((float)(config->rangeMin)/1000,(float)(config->rangeMax)/1000);
                panelCursors->cursorVerBDial->updateRange((float)(config->rangeMin)/1000,(float)(config->rangeMax)/1000);
            }

            if(config->cursorsActiveIndex == 2){
               // setVerticalCursors(config->cursorChannelIndex);
               // chart->setVerticalCursor(config->cursorChannelIndex,(panelCursors->cursorVerADial->getRealValue()+config->channelOffset[config->cursorChannelIndex])/config->channelScale[config->cursorChannelIndex],Cursor::CURSOR_A);
               // chart->setVerticalCursor(config->cursorChannelIndex,(panelCursors->cursorVerBDial->getRealValue()+config->channelOffset[config->cursorChannelIndex])/config->channelScale[config->cursorChannelIndex],Cursor::CURSOR_B);
            }
            updateCursorReadings();
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
    labelInfoPanel->setChannelScale(4,LabelFormator::formatOutout(config->channelScale[4],"V/d"));
}

void ScopeWindow::setDataMinMaxTimeAndZoom(qreal minX, qreal maxX, qreal zoom){
    chart->setDataMinMax(minX,maxX);
    chart->setZoom(zoom);
}

void ScopeWindow::channelVerticalCallback(int index){
    config->selectedChannelIndexVertical = index;
    panelSet->dialVerticalScale->setColor(Graphics::getChannelColor(index));
    if(config->channelScaleIndex[index]==-1){
        config->channelScaleIndex[index] = panelSet->dialVerticalScale->getDefaultIndex();
        config->channelOffsetIndex[index] = panelSet->dialVerticalShift->getDefaultRealValue();
    }
    panelSet->dialVerticalScale->setSelectedIndex(config->channelScaleIndex[index]);

    panelSet->dialVerticalShift->setColor(Graphics::getChannelColor(index));
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
    cursorValueVerCallback();
}

void ScopeWindow::timeBaseCallback(float value){
    updateChartTimeScale(value);
    emit timeBaseChanged(value);
}
void ScopeWindow::SamplingFreqCustomInputCallback(int freq){
    emit samlingFrequecyCustomInputChanged(freq);
}

void ScopeWindow::dataLengthInputCallback(int length)
{
    if (length>=100) emit memoryPolicyChanged(length);
}

void ScopeWindow::memoryPolicyCallback(int index){
    emit memoryPolicyChanged(index);
}

void ScopeWindow::memoryCustomLengthCallback(int number)
{
    emit memoryCustomLengthChanged(number);
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

    panelSet->dialPretrigger->setColor(Graphics::getChannelColor(index));
    panelSet->dialTriggerValue->setColor(Graphics::getChannelColor(index));
    panelSet->buttonsTriggerEdge->setColor(Graphics::getChannelColor(index),0);
    panelSet->buttonsTriggerEdge->setColor(Graphics::getChannelColor(index),1);
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
            panelSet->buttonsTriggerMode->setColor(Graphics::palette().warning,0);
            emit triggerModeChanged(ScopeTriggerMode::TRIG_STOP);
            panelSet->buttonsTriggerMode->setText("Single",0);
            labelInfoPanel->setTriggerLabelText("");

        }else if(panelSet->buttonsTriggerMode->getText(0)=="Single"){
            emit triggerModeChanged(ScopeTriggerMode::TRIG_SINGLE);
            panelSet->buttonsTriggerMode->setColor(Graphics::palette().running,0);
            panelSet->buttonsTriggerMode->setText("Stop",0);
        }
    }else if(index==1){
        emit triggerModeChanged(ScopeTriggerMode::TRIG_NORMAL);
        panelSet->buttonsTriggerMode->setColor(Graphics::palette().warning,0);
        panelSet->buttonsTriggerMode->setText("Stop",0);
    }else if (index==2){
        emit triggerModeChanged(ScopeTriggerMode::TRIG_AUTO);
        panelSet->buttonsTriggerMode->setColor(Graphics::palette().warning,0);
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
    fftChannelIndex = channelIndex;
    ChartFFTData.clear();
    if(length !=0){
        chartFFT->show();
        chartFFT->clearAll();
        ui->widget_top->hide();
        config->FFTenabled = true;
    }else{
        chartFFT->hide();
        config->FFTenabled = false;
        ui->widget_top->show();
    }
}

void ScopeWindow::fftchartChangedCallback(qreal scale, qreal shift, bool isLog)
{
    qreal range = scale * 8;
    qreal min = 0-shift;
    qreal max = range-shift;

    if(isLog){
        min -= 4*scale;
        max -=4*scale;
    }
    chartFFT->setRangeY(min,max);
    panelCursors->cursorFFTVerADial->updateRange(min,max);
    panelCursors->cursorFFTVerBDial->updateRange(min,max);

    QString unit = isLog?"dB":"Vrms";
    panelCursors->cursorFFTVerADial->updateBaseUnit(unit);
    panelCursors->cursorFFTVerBDial->updateBaseUnit(unit);

    config->FFTisLog = isLog;
}

void ScopeWindow::mathTypeChangedCallback(int index)
{
    Q_UNUSED(index);
    cursorTypeCallback(panelCursors->cursorTypeButtons->getSelectedIndex());
}

void ScopeWindow::sliderShiftCallback(int value){
    chart->setShift((float)value/10);
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
    chartFFT->clearAllCursors();
    panelCursors->ValidatePanelGUI(index);
    config->cursorsActiveIndex = index;

    if(index==0){
        labelInfoPanel->hideCursorReadings();
        chart->clearAllCursors();
        chartFFT->clearAllCursors();
    }
    if(index!=0){
        cursorChannelCallback(config->cursorChannelIndex);
    }
}

void ScopeWindow::cursorChannelCallback(int index)
{
    config->cursorChannelIndex = index;
    panelCursors->ValidatePanelGUI(config->cursorsActiveIndex);
    if(config->cursorsActiveIndex == 2){
        setVerticalCursors(config->cursorChannelIndex);
    }
    if(config->cursorsActiveIndex == 1){
        setHorizontalCursors(config->cursorChannelIndex);
    }
    labelInfoPanel->setCursorReadingsColor(config->cursorChannelIndex);
    updateCursorReadings();
}

void ScopeWindow::cursorValueHorCallback()
{
    if(config->cursorsActiveIndex == 1){
        setHorizontalCursors(config->cursorChannelIndex);
        updateCursorReadings();
    }
}

void ScopeWindow::cursorValueVerCallback()
{
    if(config->cursorsActiveIndex == 2){
        setVerticalCursors(config->cursorChannelIndex);
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
    qreal curA, curB;
    if(config->cursorsActiveIndex == 2){
        labelInfoPanel->hideCursorReadingsHor();
        if(config->cursorChannelIndex == 4){
            curA = panelCursors->cursorFFTVerADial->getRealValue();
            curB = panelCursors->cursorFFTVerBDial->getRealValue();
            QString unit = config->FFTisLog?"dB":"Vrms";
            labelInfoPanel->setCursorFFTAmplitudeReadings(curA,curB,unit);
        }else{
            curA = panelCursors->cursorVerADial->getRealValue();
            curB = panelCursors->cursorVerBDial->getRealValue();
            labelInfoPanel->setCursorVoltageReadings(curA,curB);
        }
    }
    if(config->cursorsActiveIndex == 1){
        if(config->cursorChannelIndex == 4){
            curA = chartFFT->getSignalValue(0,panelCursors->cursorFFTHorADial->getRealValue());
            curB = chartFFT->getSignalValue(0,panelCursors->cursorFFTHorBDial->getRealValue());
            labelInfoPanel->setCursorFFTFreqReadings(panelCursors->cursorFFTHorADial->getRealValue(),panelCursors->cursorFFTHorBDial->getRealValue());
            QString unit = config->FFTisLog?"dB":"Vrms";
            labelInfoPanel->setCursorFFTAmplitudeReadings(curA,curB,unit);
        }else{
            curA = chart->getSignalValue(config->cursorChannelIndex,panelCursors->cursorHorADial->getRealValue());
            curB = chart->getSignalValue(config->cursorChannelIndex,panelCursors->cursorHorBDial->getRealValue());
            curA = (curA * config->channelScale[config->cursorChannelIndex])-config->channelOffset[config->cursorChannelIndex];
            curB = (curB * config->channelScale[config->cursorChannelIndex])-config->channelOffset[config->cursorChannelIndex];
            labelInfoPanel->setCursorVoltageReadings(curA,curB);
            labelInfoPanel->setCursorTimeReadings(panelCursors->cursorHorADial->getRealValue(),panelCursors->cursorHorBDial->getRealValue());
        }
    }
}

void ScopeWindow::setHorizontalCursors(int channelIndex){
    qreal value;
    if(config->FFTenabled && panelCursors->channelButtons->getSelectedIndex()==4){
        value = panelCursors->cursorFFTHorADial->getRealValue();
        chartFFT->setHorizontalCursor(channelIndex, value, Cursor::CURSOR_A);
        value = panelCursors->cursorFFTHorBDial->getRealValue();
        chartFFT->setHorizontalCursor(channelIndex, value, Cursor::CURSOR_B);
        chart->clearAllCursors();
    }else{
        value = panelCursors->cursorHorADial->getRealValue();
        chart->setHorizontalCursor(channelIndex, value, Cursor::CURSOR_A);
        value = panelCursors->cursorHorBDial->getRealValue();
        chart->setHorizontalCursor(channelIndex, value, Cursor::CURSOR_B);
        chartFFT->clearAllCursors();
    }
}

void ScopeWindow::setVerticalCursors(int channelIndex)
{
    if(config->FFTenabled && panelCursors->channelButtons->getSelectedIndex()==4){
        chartFFT->setVerticalCursor(channelIndex,panelCursors->cursorFFTVerADial->getRealValue(),Cursor::CURSOR_A);
        chartFFT->setVerticalCursor(channelIndex,panelCursors->cursorFFTVerBDial->getRealValue(),Cursor::CURSOR_B);
        chart->clearAllCursors();
    }else{
        chart->setVerticalCursor(channelIndex,(panelCursors->cursorVerADial->getRealValue()+config->channelOffset[channelIndex])/config->channelScale[channelIndex],Cursor::CURSOR_A);
        chart->setVerticalCursor(channelIndex,(panelCursors->cursorVerBDial->getRealValue()+config->channelOffset[channelIndex])/config->channelScale[channelIndex],Cursor::CURSOR_B);
        chartFFT->clearAllCursors();
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

void ScopeWindow::updateFFTchart(QVector<QPointF> fftTrace, qreal maxFreq)
{
    ChartFFTData = fftTrace;
    chartFFT->clearAll();
    chartFFT->updateTrace(&ChartFFTData,0);
    chartFFT->setTraceColor(0,Graphics::getChannelColor(fftChannelIndex));
    chartFFT->setDataMinMax(0,maxFreq);
    // qDebug () << "FFT was calculated and received by window" << fftTrace.length();
}

void ScopeWindow::mathError(int errorPosition)
{
    panelMath->symbolicError(errorPosition);
}

void ScopeWindow::restoreGUIAfterStartup()
{
    ui->sliderSignal->setValue((chart->getShift()*2000)-1000);

    int vertical = panelSet->buttonsChannelVertical->getSelectedIndex();
    panelSet->dialVerticalScale->setColor(Graphics::getChannelColor(vertical));
    panelSet->dialVerticalShift->setColor(Graphics::getChannelColor(vertical));

    channelEnableCallback(panelSet->buttonsChannelEnable->getStatus());
    cursorTypeCallback(panelCursors->cursorTypeButtons->getSelectedIndex());
    triggerChannelCallback(panelSet->buttonsTriggerChannel->getSelectedIndex());
    resolutionChangedCallback(panelAdvanced->resolutionButtons->getSelectedIndex());

    panelMeas->setMeasButtonsColor(panelMeas->channelButtons->getSelectedIndex());
    panelMath->typeChanged(panelMath->mathType->getSelectedIndex());
    if(panelSet->buttonsTriggerMode->getSelectedIndex()==0){
        panelSet->buttonsTriggerMode->setColor(Graphics::palette().running,0);
        panelSet->buttonsTriggerMode->setText("Stop",0);
    }
}

void ScopeWindow::singleSamplingDone(){
    panelSet->buttonsTriggerMode->setText("Single",0);
    panelSet->buttonsTriggerMode->setColor(Graphics::palette().warning,0);
    labelInfoPanel->setTriggerLabelText("");
}

void ScopeWindow::samplingOngoing(){
    labelInfoPanel->setTriggerLabelText("Sampling");
}

void ScopeWindow::triggerCaptured(){
    labelInfoPanel->setTriggerLabelText("Trig");
}

void ScopeWindow::setRealSamplingRateAndLlength(int smpl, int len){
    labelInfoPanel->setSamplingRateLabelText(LabelFormator::formatOutout(smpl,"SPS") + "  ("+LabelFormator::formatOutout(len,"")+")");
    panelAdvanced->samplingFrequencyReal->setValue(LabelFormator::formatOutout(smpl,"SPS",4));
}

void ScopeWindow::setNumChannels(int channels){
    panelMeas->setNumChannels(channels);
    for (int i = channels;i<MAX_SCOPE_CHANNELS ;i++ ) {
        panelSet->buttonsChannelEnable->setButtonHidden(true,i);
        panelSet->buttonsChannelVertical->setButtonHidden(true,i);
        panelCursors->channelButtons->setButtonHidden(true,i);
        panelMath->btnChannelFFTSel->setButtonHidden(true,i);
    }
}

void ScopeWindow::updateChartTimeScale(float timeBase){
    if(config->timeBase !=0 && config->timeBase != timeBase){
        chart->setZoom(chart->getZoom()*config->timeBase/timeBase);
    }
    if(chart->getLocalZoom()!=1.0){
        labelInfoPanel->setStyleSheet("QLabel#label_scale {color:"+Graphics::palette().warning+";}");
    }else{
        labelInfoPanel->setStyleSheet("color:"+Graphics::palette().textAll+";");
    }
    labelInfoPanel->setScaleLabelText(LabelFormator::formatOutout(timeBase/chart->getLocalZoom(),"s/d"));
}


