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

    infoPanel = new widgetLabelArea(ui->widget_info);
    ui->verticalLayout_info->addWidget(infoPanel);



    //********************* insert top options *********************
    widgetTab *tabs = new widgetTab(ui->widget_settings,4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("Set",0);
    tabs->setText("Meas",1);
    tabs->setText("Disp",2);
    tabs->setText("Math",3);



    //************************* creating widget general settings *******************

    //add items - only specified widgets like buttons, dials, labels, separators....
    WidgetSeparator *separatorChannelEnable = new WidgetSeparator(tabs,"Channel enable");
    tabs->getLayout(0)->addWidget(separatorChannelEnable);

    buttonsChannelEnable = new WidgetButtons(tabs,4,ButtonTypes::CHECKABLE);
    tabs->getLayout(0)->addWidget(buttonsChannelEnable);
    buttonsChannelEnable->setText("CH1",0);
    buttonsChannelEnable->setColor(BUTTON_COLOR_ORANGE,0);
    buttonsChannelEnable->setText("CH2",1);
    buttonsChannelEnable->setColor(BUTTON_COLOR_BLUE,1);
    buttonsChannelEnable->setText("CH3",2);
    buttonsChannelEnable->setColor(BUTTON_COLOR_GREEN,2);
    buttonsChannelEnable->setText("CH4",3);
    buttonsChannelEnable->setColor(BUTTON_COLOR_PURPLE,3);



    WidgetSeparator *separatorTrig = new WidgetSeparator(tabs,"Trigger");
    tabs->getLayout(0)->addWidget(separatorTrig);

    buttonsTriggerMode = new WidgetButtons(tabs,3,ButtonTypes::RADIO,"",2);
    tabs->getLayout(0)->addWidget(buttonsTriggerMode);
    buttonsTriggerMode->setText("Stop",0);
    buttonsTriggerMode->setColor(BUTTON_COLOR_ORANGE,0);
    buttonsTriggerMode->setText("Normal",1);
    buttonsTriggerMode->setText("Auto",2);

    buttonsTriggerChannel = new WidgetButtons(tabs,4,ButtonTypes::RADIO,"Channel");
    tabs->getLayout(0)->addWidget(buttonsTriggerChannel);
    buttonsTriggerChannel->setText("CH1",0);
    buttonsTriggerChannel->setText("CH2",1);
    buttonsTriggerChannel->setText("CH3",2);
    buttonsTriggerChannel->setText("CH4",3);
    buttonsTriggerChannel->disableAll();
    buttonsTriggerChannel->setDisabledButton(false,0);

    buttonsTriggerEdge = new WidgetButtons(tabs,2,ButtonTypes::RADIO,"Edge");
    tabs->getLayout(0)->addWidget(buttonsTriggerEdge);
    buttonsTriggerEdge->setText("Rise",0);
    buttonsTriggerEdge->setText("Fall",1);

    dialPretrigger = new WidgetDialRange(tabs ,"Pretrigger");
    dialPretrigger->setRange(0,100,"%",1,1,50);
    dialPretrigger->hideUnitSelection();
    tabs->getLayout(0)->addWidget(dialPretrigger);

    dialTriggerValue = new WidgetDialRange(tabs ,"Level");
    dialTriggerValue->setRange(0,3.3,"V",10,0.01,1.65);
    dialTriggerValue->hideUnitSelection();
    tabs->getLayout(0)->addWidget(dialTriggerValue);



    WidgetSeparator *separatorHorizontal = new WidgetSeparator(tabs,"Horizontal");
    tabs->getLayout(0)->addWidget(separatorHorizontal);

    dialTimeBase = new WidgetDial(tabs ,"Time base");
    tabs->getLayout(0)->addWidget(dialTimeBase);
    fillTimeBase();
    dialTimeBase->setSelected(9);

    WidgetButtons *buttonsMemorySet = new WidgetButtons(tabs,2,ButtonTypes::RADIO,"Memory");
    tabs->getLayout(0)->addWidget(buttonsMemorySet);
    buttonsMemorySet->setText("Normal",0);
    buttonsMemorySet->setText(" Long ",1);



    WidgetSeparator *separatorVertical = new WidgetSeparator(tabs,"Vertical");
    tabs->getLayout(0)->addWidget(separatorVertical);

    WidgetButtons *buttonsChannelVerticalGain = new WidgetButtons(tabs,4,ButtonTypes::RADIO);
    tabs->getLayout(0)->addWidget(buttonsChannelVerticalGain);
    buttonsChannelVerticalGain->setText("CH1",0);
    buttonsChannelVerticalGain->setText("CH2",1);
    buttonsChannelVerticalGain->setText("CH3",2);
    buttonsChannelVerticalGain->setText("CH4",3);

    WidgetDial *dialVerticalGain = new WidgetDial(tabs ,"Scale");
    tabs->getLayout(0)->addWidget(dialVerticalGain);
    dialVerticalGain->addOption("10","mV/div",0.01);
    dialVerticalGain->addOption("20","mV/div",0.02);
    dialVerticalGain->addOption("50","mV/div",0.05);
    dialVerticalGain->addOption("100","mV/div",0.1);
    dialVerticalGain->addOption("200","mV/div",0.2);
    dialVerticalGain->addOption("500","mV/div",0.5);
    dialVerticalGain->addOption("1","V/div",1);
    dialVerticalGain->addOption("2","V/div",2);
    dialVerticalGain->setSelected(6);

    WidgetDialRange *dialVerticalShift = new WidgetDialRange(tabs ,"Shift");
    dialVerticalShift->setRange(-3.3,6.6,"V",10,0.01,0);
    //dialVerticalShift->hideUnitSelection();
    tabs->getLayout(0)->addWidget(dialVerticalShift);



    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    tabs->getLayout(0)->addItem(verticalSpacer);



    //************************* creating widget measurement *******************

    WidgetSeparator *sepa = new WidgetSeparator(tabs,"Measurement");
    tabs->getLayout(1)->addWidget(sepa);



    WidgetSeparator *separator = new WidgetSeparator(tabs,"Separator - just some stuff to test");
    tabs->getLayout(1)->addWidget(separator);

    WidgetButtons *butrad = new WidgetButtons(tabs,4,ButtonTypes::RADIO,"Selection");
    tabs->getLayout(1)->addWidget(butrad);
    butrad->setText("CH1",0);
    butrad->setColor(BUTTON_COLOR_ORANGE,0);
    butrad->setText("CH2",1);
    butrad->setColor(BUTTON_COLOR_BLUE,1);
    butrad->setText("CH3",2);
    butrad->setColor(BUTTON_COLOR_GREEN,2);
    butrad->setText("CH4",3);




    WidgetDialRange *valb = new WidgetDialRange(tabs ,"Freq");
    valb->setRange(1000,10000000,"Hz",1,1000,10000,true);
    tabs->getLayout(1)->addWidget(valb);



    WidgetDialRange *vala = new WidgetDialRange(tabs ,"Voltage");
    vala->setRange(-3.3,3.3,"V",10,0.01);
    tabs->getLayout(1)->addWidget(vala);


    WidgetSwitch *wsw = new WidgetSwitch(tabs,"prvni");
    tabs->getLayout(1)->addWidget(wsw);

    WidgetLabel *wlb = new WidgetLabel(tabs, "Channel y enable");
    tabs->getLayout(1)->addWidget(wlb);



    WidgetSelection *wsel = new WidgetSelection(tabs,"Gain");
    wsel->addOption("1x",1);
    wsel->addOption("2x",2);
    wsel->addOption("4x",4);
    tabs->getLayout(1)->addWidget(wsel);

    WidgetButtons *but2 = new WidgetButtons(tabs,1,ButtonTypes::NORMAL,"Peak to peak");
    tabs->getLayout(1)->addWidget(but2);
    but2->setText("  Add  ",0);

    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    tabs->getLayout(1)->addItem(verticalSpacer);


    //*********************** end of widgets **************




    connect(dialTimeBase,SIGNAL(valueChanged(float)),this,SLOT(timeBaseCallback(float)));
    connect(dialPretrigger,SIGNAL(valueChanged(float)),this,SLOT(pretriggerCallback(float)));
    connect(buttonsChannelEnable,SIGNAL(statusChanged(int)),this,SLOT(channelEnableCallback(int)));
    connect(buttonsTriggerMode,SIGNAL(clicked(int)),this,SLOT(triggerModeCallback(int)));
    connect(buttonsTriggerEdge,SIGNAL(clicked(int)),this,SLOT(triggerEdgeCallback(int)));
    connect(buttonsTriggerChannel,SIGNAL(clicked(int)),this,SLOT(triggerChannelCallback(int)));
    connect(dialTriggerValue,SIGNAL(valueChanged(float)),this,SLOT(triggerValueCallback(float)));


}

WindowScope::~WindowScope()
{
    delete ui;
}

void WindowScope::dataReceived(QVector<QVector<QPointF>> dataSeries, float timeBase){
    chart->clearAll();
    updateChartScale(timeBase);
    infoPanel->setTriggerLabelText("");

    for (int i = 0; i < dataSeries.length(); i++){
        if(buttonsChannelEnable->isChecked(i)){
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
        if(buttonsTriggerMode->getText(0)=="Stop"){
            buttonsTriggerMode->setColor(BUTTON_COLOR_ORANGE,0);
            emit triggerModeChanged(ScopeTriggerMode::TRIG_STOP);
            buttonsTriggerMode->setText("Single",0);
            infoPanel->setTriggerLabelText("");

        }else if(buttonsTriggerMode->getText(0)=="Single"){
            emit triggerModeChanged(ScopeTriggerMode::TRIG_SINGLE);
            buttonsTriggerMode->setColor(BUTTON_COLOR_GREEN,0);
            buttonsTriggerMode->setText("Stop",0);
        }
    }else if(index==1){
        emit triggerModeChanged(ScopeTriggerMode::TRIG_NORMAL);
        buttonsTriggerMode->setText("Stop",0);
    }else if (index==2){
        emit triggerModeChanged(ScopeTriggerMode::TRIG_AUTO);
        buttonsTriggerMode->setText("Stop",0);
    }
}


void WindowScope::channelEnableCallback(int buttonStatus){
    buttonsTriggerChannel->disableAll();

    //if current trigger channel is disabled then select channel one for trigger
    if(!(buttonsTriggerChannel->getStatus() & buttonStatus)){
        buttonsTriggerChannel->setChecked(true,0);
        triggerChannelCallback(0);
    }

    if(buttonStatus & 0x01){
        buttonsTriggerChannel->setDisabledButton(false,0);
    }
    if(buttonStatus & 0x02){
        buttonsTriggerChannel->setDisabledButton(false,1);
    }
    if(buttonStatus & 0x04){
        buttonsTriggerChannel->setDisabledButton(false,2);
    }
    if(buttonStatus & 0x08){
        buttonsTriggerChannel->setDisabledButton(false,3);
    }
    emit channelEnableChanged(buttonStatus);
}

void WindowScope::singleSamplingDone(){
    buttonsTriggerMode->setText("Single",0);
    buttonsTriggerMode->setColor(BUTTON_COLOR_ORANGE,0);
    infoPanel->setTriggerLabelText("");
}

void WindowScope::samplingOngoing(){
    infoPanel->setTriggerLabelText("Smpl");
}

void WindowScope::triggerCaptured(){
    infoPanel->setTriggerLabelText("Trig");
}

void WindowScope::setRealSamplingRate(int smpl){

    if (smpl>=100000000){
        infoPanel->setSamplingRateLabelText(QString::number(smpl/1000000,'f',0) + " MSPS");
    }else if (smpl>=10000000){
        infoPanel->setSamplingRateLabelText(QString::number(smpl/1000000,'f',1) + " MSPS");
    }else if (smpl>=1000000){
        infoPanel->setSamplingRateLabelText(QString::number(smpl/1000000,'f',2) + " MSPS");
    }else if (smpl>=100000){
        infoPanel->setSamplingRateLabelText(QString::number(smpl/1000,'f',0) + " kSPS");
    }else if (smpl>=10000){
        infoPanel->setSamplingRateLabelText(QString::number(smpl/1000,'f',1) + " kSPS");
    }else if (smpl>=1000){
        infoPanel->setSamplingRateLabelText(QString::number(smpl/1000,'f',2) + " kSPS");
    }else{
        infoPanel->setSamplingRateLabelText(QString::number(smpl,'f',0) + " SPS");
    }
}

void WindowScope::updateChartScale(float timeBase){
    chart->setXAxisMax(timeBase*10);
    //099 to mitigate precision impact
    if(timeBase<=0.00000099){
        infoPanel->setScaleLabelText(QString::number(round(timeBase*100000000000)/100,'f',2) + " ns/Div");
    }else if(timeBase<=0.00099){
        infoPanel->setScaleLabelText(QString::number(round(timeBase*100000000)/100,'f',2) + " us/Div");
    }else if(timeBase<=0.99){
        infoPanel->setScaleLabelText(QString::number(round(timeBase*100000)/100,'f',2) + " ms/Div");
    }else{
        infoPanel->setScaleLabelText(QString::number(round(timeBase*100)/100,'f',2) + " s/Div");
    }
}

void WindowScope::fillTimeBase(){
    dialTimeBase->addOption("1","us",1e-6);
    dialTimeBase->addOption("2","us",2e-6);
    dialTimeBase->addOption("5","us",5e-6);
    dialTimeBase->addOption("10","us",1e-5);
    dialTimeBase->addOption("20","us",2e-5);
    dialTimeBase->addOption("50","us",5e-5);
    dialTimeBase->addOption("100","us",1e-4);
    dialTimeBase->addOption("200","us",2e-4);
    dialTimeBase->addOption("500","us",5e-4);
    dialTimeBase->addOption("1","ms",1e-3);
    dialTimeBase->addOption("2","ms",2e-3);
    dialTimeBase->addOption("5","ms",5e-3);
    dialTimeBase->addOption("10","ms",1e-2);
    dialTimeBase->addOption("20","ms",2e-2);
    dialTimeBase->addOption("50","ms",5e-2);
    dialTimeBase->addOption("100","ms",1e-1);
    dialTimeBase->addOption("200","ms",2e-1);
    dialTimeBase->addOption("500","ms",5e-1);
    dialTimeBase->addOption("1","s",1);
    dialTimeBase->addOption("2","s",2);
    dialTimeBase->addOption("5","s",5);
    dialTimeBase->addOption("10","s",10);
}




