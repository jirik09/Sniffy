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

// Includes moved from header to implementation to reduce coupling
#include "panelsettings.h"
#include "panelmeasurement.h"
#include "panelcursors.h"
#include "panelmath.h"
#include "paneladvanced.h"
#include "scopeconfig.h"
#include "../labelformator.h"
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgetlabelarea.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgettab.h"
#include "../../GUI/widgettextinput.h"

namespace {
    inline void transformSeries(const QVector<QPointF> &src, QVector<QPointF> &dst, float offset, float invScale){
        dst.clear();
        dst.reserve(src.size());
        for(const QPointF &pt : src){
            dst.append(QPointF(pt.x(), (pt.y()+offset)*invScale));
        }
    }
}

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
    // Unified safe margins (negative margins previously caused clipping of
    // outer grid lines / top portions on some DPI & style combinations)
    chart->setMargins(0,0,0,0);
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

    connect(panelSet->dialTimeBase, &WidgetDial::valueChanged, this, &ScopeWindow::timeBaseCallback);
    // dialPretrigger is a WidgetDialRange emitting valueChanged(float,int)
    connect(panelSet->dialPretrigger, &WidgetDialRange::valueChanged, this, &ScopeWindow::pretriggerCallback);
    connect(panelSet->buttonsChannelEnable, &WidgetButtons::statusChanged, this, &ScopeWindow::channelEnableCallback);
    connect(panelSet->buttonsTriggerMode, &WidgetButtons::clicked, this, &ScopeWindow::triggerModeCallback);
    connect(panelSet->buttonsTriggerEdge, &WidgetButtons::clicked, this, &ScopeWindow::triggerEdgeCallback);
    connect(panelSet->buttonsTriggerChannel, &WidgetButtons::clicked, this, &ScopeWindow::triggerChannelCallback);
    // dialTriggerValue is a WidgetDialRange emitting valueChanged(float,int)
    connect(panelSet->dialTriggerValue, &WidgetDialRange::valueChanged, this, &ScopeWindow::triggerValueCallback);
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

    connect(panelAdvanced->modeButtons, &WidgetButtons::clicked, this, &ScopeWindow::modeChangedCallback);
    connect(panelAdvanced->channelXButtons, &WidgetButtons::clicked, this, &ScopeWindow::channelXChangedCallback);
    connect(panelAdvanced->channelYButtons, &WidgetButtons::clicked, this, &ScopeWindow::channelYChangedCallback);

    //connect top slider and chart and other stuff
    connect(ui->sliderSignal, &QSlider::valueChanged, this, &ScopeWindow::sliderShiftCallback);
    connect(chart,&widgetChart::localZoomChanged,this,&ScopeWindow::chartLocalZoomCallback);

    initTransformCache();
    Q_ASSERT(transformCache.size() == TOTAL_SCOPE_TRACES);
}

ScopeWindow::~ScopeWindow()
{
    delete ui;
}

void ScopeWindow::paintEvent(QPaintEvent *event){
    int handleW = ui->sliderSignal->size().width()/chart->getZoom()/chart->getLocalZoom();
    // Build slider stylesheet once per paint with cached backgroundButton stripped of '#'
    static QString cachedHandleColor; // #baRRGGBB (alpha + rgb)
    static QString lastBase;
    const QString base = Graphics::palette().backgroundButton;
    if (base != lastBase) {
        QString rgb = base;
        rgb.remove('#');
        cachedHandleColor = "#ba" + rgb;
        lastBase = base;
    }
    ui->sliderSignal->setStyleSheet(
        "QSlider::groove:horizontal {background: url("+Graphics::getGraphicsPath()+"signalBackground.png) center;"
        "background-color: "+Graphics::palette().windowWidget+";border: 1px solid "+Graphics::palette().textLabel+";margin-top: 3px;margin-bottom: 3px;}"
        "QSlider::handle:horizontal {background: "+cachedHandleColor+";border: 2px solid "+Graphics::palette().textLabel+";margin-top: -3px;"
        "margin-bottom: -3px;border-radius: 4px;width:"+QString::number(handleW)+"px;}"
    );
    event->accept();
}


void ScopeWindow::showDataTraces(const QVector<QVector<QPointF>> &dataSeries, float timeBase, int triggerChannelIndex){
    updateChartTimeScale(timeBase);

    this->triggerChannelIndex = triggerChannelIndex;
    ChartData = dataSeries;
    invalidateAllTransforms();
    paintTraces(ChartData,ChartMathData);
}

void ScopeWindow::paintTraces(const QVector<QVector<QPointF>> &dataSeries, const QVector<QPointF> &mathSeries){
    chart->clearAll();
    labelInfoPanel->setTriggerLabelText("");
    labelInfoPanel->hideChannelLabels();

    if (xyMode && numChannels >= 2) {
        if (xyChannelX < dataSeries.size() && xyChannelY < dataSeries.size() &&
            !dataSeries[xyChannelX].isEmpty() && !dataSeries[xyChannelY].isEmpty()) {

            const QVector<QPointF>& xData = dataSeries[xyChannelX];
            const QVector<QPointF>& yData = dataSeries[xyChannelY];
            
            int len = qMin(xData.size(), yData.size());
            QVector<QPointF> xyPoints;
            xyPoints.reserve(len);

            float scaleX = config->channelScale[xyChannelX];
            if(std::fabs(scaleX) < 1e-15f) scaleX = 1.0f;
            float offsetX = config->channelOffset[xyChannelX];

            float scaleY = config->channelScale[xyChannelY];
            if(std::fabs(scaleY) < 1e-15f) scaleY = 1.0f;
            float offsetY = config->channelOffset[xyChannelY];

            for(int i=0; i<len; ++i) {
                float valX = (xData[i].y() + offsetX) / scaleX;
                float valY = (yData[i].y() + offsetY) / scaleY;
                xyPoints.append(QPointF(valX, valY));
            }
            
            chart->updateTrace(&xyPoints, 0);
            chart->setRangeX(CHART_MIN_Y, CHART_MAX_Y);
            chart->setRangeY(CHART_MIN_Y, CHART_MAX_Y);
        }
        return;
    }

    //paint math
    paintMath(mathSeries);

    //paint data traces
    for (int i = 0; i < dataSeries.length(); i++) {
        if(!panelSet->buttonsChannelEnable->isChecked(i)) continue;
        // Only real hardware channels here; math trace handled separately
        if(i >= MAX_SCOPE_CHANNELS) continue;
        Q_ASSERT(i < transformCache.size());

        float scale = config->channelScale[i];
        if(std::fabs(scale) < 1e-15f) scale = 1.0f;
        const float invScale = 1.0f / scale;
        const float offset = config->channelOffset[i];

        const QVector<QPointF>& transformed = getTransformedChannel(i, dataSeries[i]);
    chart->updateTrace(const_cast<QVector<QPointF>*>(&transformed), i);

        float zeroMarkerPosition = offset*invScale;
        if(zeroMarkerPosition>=CHART_MAX_Y)        chart-> setHorizontalMarker(i,CHART_MAX_Y,MarkerType::ARROW_UP_SMALL);
        else if(zeroMarkerPosition<=CHART_MIN_Y)   chart-> setHorizontalMarker(i,CHART_MIN_Y,MarkerType::ARROW_DOWN_SMALL);
        else                                       chart-> setHorizontalMarker(i,zeroMarkerPosition,MarkerType::TICK);

        float triggerMarkerPosition = (config->channelOffset[i]+(qreal(config->triggerLevel)/65535*(config->rangeMax-config->rangeMin)/1000))/config->channelScale[i];
        if(panelSet->buttonsTriggerChannel->getSelectedIndex()==i) {
            chart-> setHorizontalMarker(i,triggerMarkerPosition,MarkerType::TRIGGER);
        }

        labelInfoPanel->setChannelLabelVisible(i,true);
        labelInfoPanel->setChannelScale(i,LabelFormator::formatOutout(config->channelScale[i],"V/d"));

        if(config->FFTenabled){
            panelCursors->cursorFFTHorADial->updateRange(0,config->realSamplingRate/2);
            panelCursors->cursorFFTHorBDial->updateRange(0,config->realSamplingRate/2);
        } else {
            panelCursors->cursorHorADial->updateRange(config->timeMin,config->timeMax);
            panelCursors->cursorHorBDial->updateRange(config->timeMin,config->timeMax);
            panelCursors->cursorVerADial->updateRange((float)(config->rangeMin)/1000,(float)(config->rangeMax)/1000);
            panelCursors->cursorVerBDial->updateRange((float)(config->rangeMin)/1000,(float)(config->rangeMax)/1000);
        }
        updateCursorReadings();
    }
    // After plotting channels, plot math if present
    paintMath(mathSeries);
    chart->setVerticalMarker(triggerChannelIndex,0);
}

// Helper: ensure current trigger channel is valid; if not, select first enabled
void ScopeWindow::validateAndApplyTriggerChannel(int buttonStatus){
    int currentTrig = panelSet->buttonsTriggerChannel->getSelectedIndex();
    if(currentTrig < 0 || currentTrig >= MAX_SCOPE_CHANNELS || !(buttonStatus & (1 << currentTrig))){
        for(int i=0;i<MAX_SCOPE_CHANNELS;i++){
            if(buttonStatus & (1<<i)){
                panelSet->buttonsTriggerChannel->setChecked(true,i);
                triggerChannelCallback(i);
                return;
            }
        }
    }
}

// Helper: enable/disable trigger channel buttons according to bit mask
void ScopeWindow::updateTriggerChannelButtons(int buttonStatus){
    for(int i=0;i<MAX_SCOPE_CHANNELS;i++){
        bool enabled = (buttonStatus & (1<<i));
        panelSet->buttonsTriggerChannel->setDisabledButton(!enabled,i);
    }
}


void ScopeWindow::paintMath(const QVector<QPointF> &mathSeries){
    if(mathSeries.isEmpty()) return;
    // Math trace index defined centrally
    const int MATH_INDEX = MATH_CHANNEL_INDEX;
    if(MATH_INDEX >= TOTAL_SCOPE_TRACES) return; // safety
    float scale = config->channelScale[MATH_INDEX];
    if(std::fabs(scale) < 1e-15f) scale = 1.0f;
    const float invScale = 1.0f / scale;
    const float offset = config->channelOffset[MATH_INDEX];
    const QVector<QPointF>& transformed = getTransformedChannel(MATH_INDEX, mathSeries);
    chart->updateTrace(const_cast<QVector<QPointF>*>(&transformed), MATH_INDEX);
    float zeroMarkerPosition = offset*invScale;
    if(zeroMarkerPosition>=CHART_MAX_Y)        chart-> setHorizontalMarker(MATH_INDEX,CHART_MAX_Y,MarkerType::ARROW_UP_SMALL);
    else if(zeroMarkerPosition<=CHART_MIN_Y)   chart-> setHorizontalMarker(MATH_INDEX,CHART_MIN_Y,MarkerType::ARROW_DOWN_SMALL);
    else                                       chart-> setHorizontalMarker(MATH_INDEX,zeroMarkerPosition,MarkerType::TICK);
    labelInfoPanel->setChannelLabelVisible(MATH_INDEX,true);
    labelInfoPanel->setChannelScale(MATH_INDEX,LabelFormator::formatOutout(config->channelScale[MATH_INDEX],"V/d"));
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
    invalidateChannelTransform(config->selectedChannelIndexVertical);
    paintTraces(ChartData,ChartMathData);
}
void ScopeWindow::channelVerticalShiftCallback(float value){
    config->channelOffset[config->selectedChannelIndexVertical] = value;
    invalidateChannelTransform(config->selectedChannelIndexVertical);
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
    // buttonStatus bit mask expected: bit0..bit3 for channels 0..3
    emit channelEnableChanged(buttonStatus);
    validateAndApplyTriggerChannel(buttonStatus);
    updateTriggerChannelButtons(buttonStatus);
    
    for(int i=0; i<MAX_SCOPE_CHANNELS; i++) {
        bool enabled = (buttonStatus & (1 << i));
        panelAdvanced->channelXButtons->setEnabledButton(enabled, i);
        panelAdvanced->channelYButtons->setEnabledButton(enabled, i);
    }

    // Repaint traces to reflect hiding/showing channels
    paintTraces(ChartData,ChartMathData);
    updateCursorReadings();
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
            if (xyMode) {
                // In XY mode, vertical cursors measure Y-axis (Voltage Y)
                // They are already set up for voltage, but we should ensure they use Y channel scale/offset?
                // Actually, vertical cursors are attached to Y-axis.
                // In XY mode, Y-axis is divisions.
                // cursorVerADial returns Volts.
                // setVerticalCursors converts Volts to Divisions using config->channelScale[channelIndex].
                // If channelIndex is not xyChannelY, it might be confusing.
                // But usually cursors are attached to a specific channel selected in Cursors panel.
                // If user selects a channel in Cursors panel, we use that channel's scale.
                // In XY mode, we are plotting Channel Y vs Channel X.
                // So we should probably force cursors to use xyChannelY?
                // Or just let the user select the channel they want to measure (which should be xyChannelY).
                // For now, let's assume standard behavior (user selects channel).
            }
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
            if (xyMode) {
                // In XY mode, horizontal cursors measure X-axis (Voltage X)
                curA = panelCursors->cursorHorADial->getRealValue();
                curB = panelCursors->cursorHorBDial->getRealValue();
                labelInfoPanel->setCursorVoltageReadingsX(curA, curB);
                
                // We could also show Y value at this X, but that requires finding the point.
                // For now, just showing X voltage is what was requested.
            } else {
                curA = chart->getSignalValue(config->cursorChannelIndex,panelCursors->cursorHorADial->getRealValue());
                curB = chart->getSignalValue(config->cursorChannelIndex,panelCursors->cursorHorBDial->getRealValue());
                curA = (curA * config->channelScale[config->cursorChannelIndex])-config->channelOffset[config->cursorChannelIndex];
                curB = (curB * config->channelScale[config->cursorChannelIndex])-config->channelOffset[config->cursorChannelIndex];
                labelInfoPanel->setCursorVoltageReadings(curA,curB);
                labelInfoPanel->setCursorTimeReadings(panelCursors->cursorHorADial->getRealValue(),panelCursors->cursorHorBDial->getRealValue());
            }
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
    } else {
        value = panelCursors->cursorHorADial->getRealValue();
        if (xyMode) {
            // Convert Voltage to Divisions for X-axis
            float scaleX = config->channelScale[xyChannelX];
            if(std::fabs(scaleX) < 1e-15f) scaleX = 1.0f;
            float offsetX = config->channelOffset[xyChannelX];
            value = (value + offsetX) / scaleX;
        }
        chart->setHorizontalCursor(channelIndex, value, Cursor::CURSOR_A);
        
        value = panelCursors->cursorHorBDial->getRealValue();
        if (xyMode) {
            // Convert Voltage to Divisions for X-axis
            float scaleX = config->channelScale[xyChannelX];
            if(std::fabs(scaleX) < 1e-15f) scaleX = 1.0f;
            float offsetX = config->channelOffset[xyChannelX];
            value = (value + offsetX) / scaleX;
        }
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
    invalidateChannelTransform(MATH_CHANNEL_INDEX);
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
    bool tmp;
    for (int i = 0; i< MAX_SCOPE_CHANNELS; i++) {
        if(channels <= i) tmp = true;
        else tmp = false;
        panelSet->buttonsChannelEnable->setButtonHidden(tmp,i);
        panelSet->buttonsChannelVertical->setButtonHidden(tmp,i);
        panelSet->buttonsTriggerChannel->setButtonHidden(tmp,i);
        panelCursors->channelButtons->setButtonHidden(tmp,i);
        panelMath->btnChannelFFTSel->setButtonHidden(tmp,i);
        panelMeas->channelButtons->setButtonHidden(tmp,i);
        panelMeas->channelButtonPhaseA->setButtonHidden(tmp,i);
        panelMeas->channelButtonPhaseB->setButtonHidden(tmp,i);
        
        panelAdvanced->channelXButtons->setButtonHidden(tmp, i);
        panelAdvanced->channelYButtons->setButtonHidden(tmp, i);
    }

    numChannels = channels;
    
    if (channels > 0) panelAdvanced->channelXButtons->setChecked(true, 0);
    if (channels > 1) panelAdvanced->channelYButtons->setChecked(true, 1);
    
    if (channels < 2) {
        panelAdvanced->modeButtons->setEnabled(false);
        if (xyMode) {
             panelAdvanced->modeButtons->setChecked(true, 0);
             modeChangedCallback(0);
        }
    } else {
        panelAdvanced->modeButtons->setEnabled(true);
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

// ---------------- Transform cache helpers ----------------
void ScopeWindow::initTransformCache(){
    transformCache.resize(TOTAL_SCOPE_TRACES);
    for(auto &c : transformCache){
        c.dirty = true;
        c.lastScale = std::numeric_limits<float>::quiet_NaN();
        c.lastOffset = std::numeric_limits<float>::quiet_NaN();
        c.transformed.clear();
    }
}

void ScopeWindow::invalidateAllTransforms(){
    for(auto &c : transformCache){
        c.dirty = true;
    }
}

void ScopeWindow::invalidateChannelTransform(int channelIndex){
    if(channelIndex < 0 || channelIndex >= transformCache.size()) return;
    transformCache[channelIndex].dirty = true;
}

const QVector<QPointF>& ScopeWindow::getTransformedChannel(int ch, const QVector<QPointF>& src){
    if(ch < 0 || ch >= transformCache.size()) return src; // fallback
    auto &cache = transformCache[ch];
    float scale = config->channelScale[ch];
    if(std::fabs(scale) < 1e-15f) scale = 1.0f;
    float offset = config->channelOffset[ch];
    // Recompute if dirty OR params changed OR source size differs
    if(cache.dirty || cache.lastScale != scale || cache.lastOffset != offset || cache.transformed.size() != src.size()){
        const float invScale = 1.0f/scale;
        transformSeries(src, cache.transformed, offset, invScale);
        cache.lastScale = scale;
        cache.lastOffset = offset;
        cache.dirty = false;
    }
    return cache.transformed;
}

void ScopeWindow::modeChangedCallback(int index) {
    bool newMode = (index == 1);
    if (newMode != xyMode) {
        if (newMode) {
            // Entering XY mode
            QList<QAbstractAxis*> axes = chart->getChart()->axes(Qt::Horizontal);
            if (!axes.isEmpty()) {
                QValueAxis *axis = qobject_cast<QValueAxis*>(axes.first());
                if (axis) {
                    savedMinX = axis->min();
                    savedMaxX = axis->max();
                }
            }
            
            // Update horizontal cursors (X-axis) to Voltage mode
            float scaleX = config->channelScale[xyChannelX];
            if(std::fabs(scaleX) < 1e-15f) scaleX = 1.0f;
            float offsetX = config->channelOffset[xyChannelX];
            
            float minV = (CHART_MIN_Y * scaleX) - offsetX;
            float maxV = (CHART_MAX_Y * scaleX) - offsetX;
            
            panelCursors->cursorHorADial->updateBaseUnit("V");
            panelCursors->cursorHorBDial->updateBaseUnit("V");
            panelCursors->cursorHorADial->updateRange(minV, maxV);
            panelCursors->cursorHorBDial->updateRange(minV, maxV);
            
        } else {
            // Leaving XY mode
            chart->setRangeX(savedMinX, savedMaxX);
            
            // Restore horizontal cursors to Time mode
            panelCursors->cursorHorADial->updateBaseUnit("s");
            panelCursors->cursorHorBDial->updateBaseUnit("s");
            panelCursors->cursorHorADial->updateRange(config->timeMin, config->timeMax);
            panelCursors->cursorHorBDial->updateRange(config->timeMin, config->timeMax);
        }
        xyMode = newMode;
    }
    paintTraces(ChartData, ChartMathData);
    updateCursorReadings();
}

void ScopeWindow::channelXChangedCallback(int index) {
    xyChannelX = index;
    if (xyMode) {
        // Update cursor ranges for new channel scale
        float scaleX = config->channelScale[xyChannelX];
        if(std::fabs(scaleX) < 1e-15f) scaleX = 1.0f;
        float offsetX = config->channelOffset[xyChannelX];
        
        float minV = (CHART_MIN_Y * scaleX) - offsetX;
        float maxV = (CHART_MAX_Y * scaleX) - offsetX;
        
        panelCursors->cursorHorADial->updateRange(minV, maxV);
        panelCursors->cursorHorBDial->updateRange(minV, maxV);
        
        paintTraces(ChartData, ChartMathData);
        updateCursorReadings();
    }
}

void ScopeWindow::channelYChangedCallback(int index) {
    xyChannelY = index;
    if (xyMode) paintTraces(ChartData, ChartMathData);
}


