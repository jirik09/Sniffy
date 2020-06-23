/*
Window for scope
left - area for charts
right - area for setting filled with different widgets

Comment: This will be similar for all features but making this window abstract made me
crazy so I keept it like this and we will have to specify it for all features like gen, counter etc.
But it can be easily coppied so we can keep some template file.
*/
#include "windowscope.h"
#include "ui_windowscope.h"

WindowScope::WindowScope(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindowScope)
{
    ui->setupUi(this);

    chart = new widgetChart(ui->widget_trace,4);
    ui->verticalLayout_trace->addWidget(chart);

    labelInfoPanel = new widgetLabelArea(ui->widget_info);
    ui->verticalLayout_info->addWidget(labelInfoPanel);



    //********************* insert top options *********************
    widgetTab *tabs = new widgetTab(ui->widget_settings,4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("Set",0);
    tabs->setText("Meas",1);
    tabs->setText("Disp",2);
    tabs->setText("Math",3);

    //************************* creating widget general settings *******************
    panelSet = new PanelSettings(tabs->getLayout(0),tabs);

    connect(panelSet->dialTimeBase,SIGNAL(valueChanged(float)),this,SLOT(timeBaseCallback(float)));
    connect(panelSet->dialPretrigger,SIGNAL(valueChanged(float)),this,SLOT(pretriggerCallback(float)));
    connect(panelSet->buttonsChannelEnable,SIGNAL(statusChanged(int)),this,SLOT(channelEnableCallback(int)));
    connect(panelSet->buttonsTriggerMode,SIGNAL(clicked(int)),this,SLOT(triggerModeCallback(int)));
    connect(panelSet->buttonsTriggerEdge,SIGNAL(clicked(int)),this,SLOT(triggerEdgeCallback(int)));
    connect(panelSet->buttonsTriggerChannel,SIGNAL(clicked(int)),this,SLOT(triggerChannelCallback(int)));
    connect(panelSet->dialTriggerValue,SIGNAL(valueChanged(float)),this,SLOT(triggerValueCallback(float)));


    //************************* creating widget measurement *******************
    panelMeas = new PanelMeasurement(tabs->getLayout(1),tabs);
}

WindowScope::~WindowScope()
{
    delete ui;
}

void WindowScope::dataReceived(QVector<QVector<QPointF>> dataSeries, float timeBase){
    chart->clearAll();
    updateChartScale(timeBase);
    labelInfoPanel->setTriggerLabelText("");

    for (int i = 0; i < dataSeries.length(); i++){
        if(panelSet->buttonsChannelEnable->isChecked(i)){
            chart->updateTrace(&dataSeries[i], i);
        }
    }
}

void WindowScope::timeBaseCallback(float value){
    emit timeBaseChanged(value);
    updateChartScale(value);
}

void WindowScope::pretriggerCallback(float value){
    emit pretriggerChanged(value);
}

void WindowScope::triggerValueCallback(float value){
    emit triggerValueChanged(value/3.3*100);
}

void WindowScope::triggerChannelCallback(int index){
    emit triggerChannelChanged(index);
}

void WindowScope::triggerEdgeCallback(int index){
    if(index==0){
        emit triggerEdgeChanged(ScopeTriggerEdge::EDGE_RISING);
    }else if(index==1){
        emit triggerEdgeChanged(ScopeTriggerEdge::EDGE_FALLING);
    }
}

void WindowScope::triggerModeCallback(int index){
    if(index==0){
        if(panelSet->buttonsTriggerMode->getText(0)=="Stop"){
            panelSet->buttonsTriggerMode->setColor(BUTTON_COLOR_ORANGE,0);
            emit triggerModeChanged(ScopeTriggerMode::TRIG_STOP);
            panelSet->buttonsTriggerMode->setText("Single",0);
            labelInfoPanel->setTriggerLabelText("");

        }else if(panelSet->buttonsTriggerMode->getText(0)=="Single"){
            emit triggerModeChanged(ScopeTriggerMode::TRIG_SINGLE);
            panelSet->buttonsTriggerMode->setColor(BUTTON_COLOR_GREEN,0);
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


void WindowScope::channelEnableCallback(int buttonStatus){
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

void WindowScope::singleSamplingDone(){
    panelSet->buttonsTriggerMode->setText("Single",0);
    panelSet->buttonsTriggerMode->setColor(BUTTON_COLOR_ORANGE,0);
    labelInfoPanel->setTriggerLabelText("");
}

void WindowScope::samplingOngoing(){
    labelInfoPanel->setTriggerLabelText("Smpl");
}

void WindowScope::triggerCaptured(){
    labelInfoPanel->setTriggerLabelText("Trig");
}

void WindowScope::setRealSamplingRate(int smpl){

    if (smpl>=100000000){
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl/1000000,'f',0) + " MSPS");
    }else if (smpl>=10000000){
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl/1000000,'f',1) + " MSPS");
    }else if (smpl>=1000000){
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl/1000000,'f',2) + " MSPS");
    }else if (smpl>=100000){
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl/1000,'f',0) + " kSPS");
    }else if (smpl>=10000){
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl/1000,'f',1) + " kSPS");
    }else if (smpl>=1000){
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl/1000,'f',2) + " kSPS");
    }else{
        labelInfoPanel->setSamplingRateLabelText(QString::number(smpl,'f',0) + " SPS");
    }
}

void WindowScope::updateChartScale(float timeBase){
    chart->setXAxisMax(timeBase*10);
    //099 to mitigate precision impact
    if(timeBase<=0.00000099){
        labelInfoPanel->setScaleLabelText(QString::number(round(timeBase*100000000000)/100,'f',2) + " ns/Div");
    }else if(timeBase<=0.00099){
        labelInfoPanel->setScaleLabelText(QString::number(round(timeBase*100000000)/100,'f',2) + " us/Div");
    }else if(timeBase<=0.99){
        labelInfoPanel->setScaleLabelText(QString::number(round(timeBase*100000)/100,'f',2) + " ms/Div");
    }else{
        labelInfoPanel->setScaleLabelText(QString::number(round(timeBase*100)/100,'f',2) + " s/Div");
    }
}


