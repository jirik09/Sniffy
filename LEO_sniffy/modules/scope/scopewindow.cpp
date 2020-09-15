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

    chart = new widgetChart(ui->widget_trace, 4);
    chart->setRange(-0.1, 0.1, -0.1, 3.9);
    chart->createHorizontalMarkes();
    ui->verticalLayout_trace->addWidget(chart);

    labelInfoPanel = new WidgetLabelArea(ui->widget_info);
    ui->verticalLayout_info->addWidget(labelInfoPanel);

    // ********************* insert top options *********************
    widgetTab *tabs = new widgetTab(ui->widget_settings,4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("Set",0);
    tabs->setText("Meas",1);
    tabs->setText("Disp",2);
    tabs->setText("Math",3);

    // ************************* creating widget general settings *******************
    panelSet = new PanelSettings(tabs->getLayout(0),tabs);

    connect(panelSet->dialTimeBase,SIGNAL(valueChanged(float)),this,SLOT(timeBaseCallback(float)));
    connect(panelSet->dialPretrigger,SIGNAL(valueChanged(float)),this,SLOT(pretriggerCallback(float)));
    connect(panelSet->buttonsChannelEnable,SIGNAL(statusChanged(int)),this,SLOT(channelEnableCallback(int)));
    connect(panelSet->buttonsTriggerMode,SIGNAL(clicked(int)),this,SLOT(triggerModeCallback(int)));
    connect(panelSet->buttonsTriggerEdge,SIGNAL(clicked(int)),this,SLOT(triggerEdgeCallback(int)));
    connect(panelSet->buttonsTriggerChannel,SIGNAL(clicked(int)),this,SLOT(triggerChannelCallback(int)));
    connect(panelSet->dialTriggerValue,SIGNAL(valueChanged(float)),this,SLOT(triggerValueCallback(float)));

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
    int handleW = ui->sliderSignal->size().width()/chart->getZoom();;
    ui->sliderSignal->setStyleSheet(QString::fromUtf8(
                                        "QSlider::groove:horizontal {background: url(:/graphics/graphics/signalBackground.png) center;"
                                        "background-color: rgb(38, 38, 38);border: 1px solid #777;margin-top: 3px;margin-bottom: 3px;}"
                                        "QSlider::handle:horizontal {background: rgba(0, 0, 0, 150);border: 2px solid #777;margin-top: -3px;"
                                        "margin-bottom: -3px;border-radius: 4px;width:")+QString::number(handleW)+QString::fromUtf8("px;}"));
    event->accept();
}

void ScopeWindow::showDataTraces(QVector<QVector<QPointF>> dataSeries, float timeBase, int triggerChannelIndex){
    updateChartScale(timeBase);
    labelInfoPanel->setTriggerLabelText("");
    labelInfoPanel->hideChannelLabels();
    paintTraces(dataSeries);

    chart->setZoom(1.2);
    chart->setHorizontalMarker(triggerChannelIndex,0);
}

void ScopeWindow::paintTraces(QVector<QVector<QPointF>> dataSeries){
    chart->clearAll();
    for (int i = 0; i < dataSeries.length(); i++){
        if(panelSet->buttonsChannelEnable->isChecked(i)){
            chart->updateTrace(&dataSeries[i], i);
            labelInfoPanel->setChannelLabelVisible(i,true);
        }
    }
}

void ScopeWindow::setDataMinMaxTime(qreal minX, qreal maxX){
     chart->setDataMinMax(minX,maxX);
}

void ScopeWindow::timeBaseCallback(float value){
    emit timeBaseChanged(value);
    updateChartScale(value);
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
            panelSet->buttonsTriggerMode->setColor(BCKGRND_COLOR_ORANGE,0);
            emit triggerModeChanged(ScopeTriggerMode::TRIG_STOP);
            panelSet->buttonsTriggerMode->setText("Single",0);
            labelInfoPanel->setTriggerLabelText("");

        }else if(panelSet->buttonsTriggerMode->getText(0)=="Single"){
            emit triggerModeChanged(ScopeTriggerMode::TRIG_SINGLE);
            panelSet->buttonsTriggerMode->setColor(BCKGRND_COLOR_GREEN,0);
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
    chart->setShift(value);
}

void ScopeWindow::updateMeasurement(QList<Measurement*> m){
    labelInfoPanel->setMeasurements(m);
}

void ScopeWindow::singleSamplingDone(){
    panelSet->buttonsTriggerMode->setText("Single",0);
    panelSet->buttonsTriggerMode->setColor(BCKGRND_COLOR_ORANGE,0);
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

void ScopeWindow::updateChartScale(float timeBase){
  //  chart->setXAxisMax(timeBase*10);
    labelInfoPanel->setScaleLabelText(LabelFormator::formatOutout(timeBase,"s/div"));
}


