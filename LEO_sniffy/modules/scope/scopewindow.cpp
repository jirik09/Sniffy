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

ScopeWindow::ScopeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScopeWindow)
{
    ui->setupUi(this);

    ui->verticalSlider->hide();
    ui->verticalSlider_2->hide();

    chart = new widgetChart(ui->widget_chart, 4);
    chart->setRange(-0.1, 0.1, CHART_MIN_Y, CHART_MAX_Y);
    chart->enableLocalMouseZoom();

    ui->verticalLayout_chart->addWidget(chart);

    labelInfoPanel = new WidgetLabelArea(ui->widget_info);
    ui->verticalLayout_info->addWidget(labelInfoPanel);

    // ********************* insert top options *********************
    widgetTab *tabs = new widgetTab(ui->widget_settings,4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("Set",0);
    tabs->setText("Meas",1);
    tabs->setText("Cursors",2);
    tabs->setText("Math",3);

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

    connect(chart,&widgetChart::localZoomChanged,this,&ScopeWindow::chartLocalZoomCallback);



    // ************************* creating widget measurement *******************
    panelMeas = new PanelMeasurement(tabs->getLayout(1),tabs);

    connect(panelMeas, &PanelMeasurement::measurementAdded, this,&ScopeWindow::measurementAddedCallback);
    connect(panelMeas, &PanelMeasurement::measurementClearAll, this, &ScopeWindow::measurementClearCallback);


    connect(ui->sliderSignal, &QSlider::valueChanged, this, &ScopeWindow::sliderShiftCallback);
}

ScopeWindow::~ScopeWindow()
{
    delete ui;
}

void ScopeWindow::paintEvent(QPaintEvent *event){
    int handleW = ui->sliderSignal->size().width()/chart->getZoom()/chart->getLocalZoom();
    ui->sliderSignal->setStyleSheet(QString::fromUtf8(
                                        "QSlider::groove:horizontal {background: url(:/graphics/graphics/signalBackground.png) center;"
                                        "background-color: ")+BACKGROUND_COLOR_DATA_AREA+"border: 1px solid #777;margin-top: 3px;margin-bottom: 3px;}"
                                        "QSlider::handle:horizontal {background: rgba(0, 0, 0, 150);border: 2px solid #777;margin-top: -3px;"
                                        "margin-bottom: -3px;border-radius: 4px;width:"+QString::number(handleW)+QString::fromUtf8("px;}"));
    event->accept();
}


void ScopeWindow::showDataTraces(QVector<QVector<QPointF>> dataSeries, float timeBase, int triggerChannelIndex){
    updateChartTimeScale(timeBase);
    labelInfoPanel->setTriggerLabelText("");
    labelInfoPanel->hideChannelLabels();

    this->triggerChannelIndex = triggerChannelIndex;
    ChartData = dataSeries;
    paintTraces(ChartData);
}

void ScopeWindow::paintTraces(QVector<QVector<QPointF>> dataSeries){
    chart->clearAll();
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

            labelInfoPanel->setChannelLabelVisible(i,true);
            labelInfoPanel->setChannelScale(i,LabelFormator::formatOutout(config->channelScale[i],"V/div"));
        }
    }
    chart->setVerticalMarker(triggerChannelIndex,0);
}

void ScopeWindow::setDataMinMaxTimeAndZoom(qreal minX, qreal maxX, qreal zoom){
     chart->setDataMinMax(minX,maxX);
     chart->setZoom(zoom*1.2);
}

void ScopeWindow::channelVerticalCallback(int index){
    config->selectedChannelIndexVertical = index;
    QString style = QString::fromUtf8("color:"+Colors::getChannelColorString(index));
    QString styleBcg = QString::fromUtf8("background-color:"+Colors::getChannelColorString(index));

    panelSet->dialVerticalScale->setDialColor(style);
    panelSet->dialVerticalScale->setDialButtonsColor(styleBcg);
    if(config->channelScaleIndex[index]==-1){
        config->channelScaleIndex[index] = panelSet->dialVerticalScale->getDefaultIndex();
        config->channelOffsetIndex[index] = panelSet->dialVerticalShift->getDefaultRealValue();
    }
    panelSet->dialVerticalScale->setSelectedIndex(config->channelScaleIndex[index]);

    panelSet->dialVerticalShift->setDialColor(style);
    panelSet->dialVerticalShift->setDialButtonsColor(styleBcg);
    panelSet->dialVerticalShift->setRealValue(config->channelOffset[index]);
}

void ScopeWindow::channelVerticalScaleCallback(float value){
    config->channelScale[config->selectedChannelIndexVertical] = value;
    config->channelScaleIndex[config->selectedChannelIndexVertical] = panelSet->dialVerticalScale->getSelectedIndex();
    paintTraces(ChartData);
}
void ScopeWindow::channelVerticalShiftCallback(float value){
    config->channelOffset[config->selectedChannelIndexVertical] = value;
    paintTraces(ChartData);
}

void ScopeWindow::timeBaseCallback(float value){
    emit timeBaseChanged(value);
    updateChartTimeScale(value);
    previousTimeBase = value;
}

void ScopeWindow::longMemoryCallback(int index){
    emit memoryLengthChanged(index);
}

void ScopeWindow::pretriggerCallback(float value){
    emit pretriggerChanged(value);
}

void ScopeWindow::triggerValueCallback(float value){
    emit triggerValueChanged(value/3.3*100);
}

void ScopeWindow::triggerChannelCallback(int index){
    emit triggerChannelChanged(index);
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
            panelSet->buttonsTriggerMode->setColor("background-color:"+QString::fromUtf8(COLOR_ORANGE),0);
            emit triggerModeChanged(ScopeTriggerMode::TRIG_STOP);
            panelSet->buttonsTriggerMode->setText("Single",0);
            labelInfoPanel->setTriggerLabelText("");

        }else if(panelSet->buttonsTriggerMode->getText(0)=="Single"){
            emit triggerModeChanged(ScopeTriggerMode::TRIG_SINGLE);
            panelSet->buttonsTriggerMode->setColor("background-color:"+QString::fromUtf8(COLOR_GREEN),0);
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

void ScopeWindow::updateMeasurement(QList<Measurement*> m){
    labelInfoPanel->setMeasurements(m);
}

void ScopeWindow::passConfig(ScopeConfig &conf)
{
    this->config = &conf;
}

void ScopeWindow::singleSamplingDone(){
    panelSet->buttonsTriggerMode->setText("Single",0);
    panelSet->buttonsTriggerMode->setColor("background-color:"+QString::fromUtf8(COLOR_ORANGE),0);
    labelInfoPanel->setTriggerLabelText("");
}

void ScopeWindow::samplingOngoing(){
    labelInfoPanel->setTriggerLabelText("Sampling");
}

void ScopeWindow::triggerCaptured(){
    labelInfoPanel->setTriggerLabelText("Trig");
}

void ScopeWindow::setRealSamplingRate(int smpl){
    labelInfoPanel->setSamplingRateLabelText(LabelFormator::formatOutout(smpl,"SPS"));
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

    labelInfoPanel->setScaleLabelText(LabelFormator::formatOutout(timeBase/chart->getLocalZoom(),"s/div"));
}


