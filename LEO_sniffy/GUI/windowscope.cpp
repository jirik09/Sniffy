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


    //********************* insert top options *********************
    WidgetTopSelection = new WidgetTop(ui->widget_settings,4);
    WidgetTopSelection->setText("Set",0);
    WidgetTopSelection->setText("Meas",1);
    WidgetTopSelection->setText("Disp",2);
    WidgetTopSelection->setSelected(0);

    ui->verticalLayout_settings->addWidget(WidgetTopSelection);

    //link options from topWidget
    connect(WidgetTopSelection->getPushButton(0),SIGNAL(clicked()),this,SLOT(generalSettingsClicked()));
    connect(WidgetTopSelection->getPushButton(1),SIGNAL(clicked()),this,SLOT(generalMeasClicked()));

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);  //horizontal , vertical


    //************************* creating widget general settings *******************

    //create scroll area - each seting selection has its own scroll area and other stuff
    scrollAreaGeneral = new QScrollArea(ui->widget_settings);
    scrollAreaGeneral->setWidgetResizable(true);
    scrollAreaGeneral->setSizePolicy(sizePolicy);

    WidgetGeneral = new QWidget();
    WidgetGeneral->setSizePolicy(sizePolicy);
    verticalLayoutGeneral = new QVBoxLayout(WidgetGeneral);

    scrollAreaGeneral->setContentsMargins(0,0,0,0);
    verticalLayoutGeneral->setSpacing(3);
    verticalLayoutGeneral->setContentsMargins(10,0,10,0);

    scrollAreaGeneral->setWidget(WidgetGeneral);
    ui->verticalLayout_settings->addWidget(scrollAreaGeneral);

    //add items - only specified widgets like buttons, dials, labels, separators....
    WidgetSeparator *separatorChannelEnable = new WidgetSeparator(WidgetGeneral,"Channel enable");
    verticalLayoutGeneral->addWidget(separatorChannelEnable);

    WidgetButtons *buttonsChannelEnable = new WidgetButtons(WidgetGeneral,4,ButtonTypes::CHECKABLE);
    verticalLayoutGeneral->addWidget(buttonsChannelEnable);
    buttonsChannelEnable->setText("CH1",0);
    buttonsChannelEnable->setColor(BACKGROUND_COLOR_ORANGE,0);
    buttonsChannelEnable->setText("CH2",1);
    buttonsChannelEnable->setColor(BACKGROUND_COLOR_BLUE,1);
    buttonsChannelEnable->setText("CH3",2);
    buttonsChannelEnable->setColor(BACKGROUND_COLOR_GREEN,2);
    buttonsChannelEnable->setText("CH4",3);
    buttonsChannelEnable->setColor(BACKGROUND_COLOR_PURPLE,3);



    WidgetSeparator *separatorTrig = new WidgetSeparator(WidgetGeneral,"Trigger");
    verticalLayoutGeneral->addWidget(separatorTrig);

    WidgetButtons *buttonsTriggerMode = new WidgetButtons(WidgetGeneral,3,ButtonTypes::RADIO,"",2);
    verticalLayoutGeneral->addWidget(buttonsTriggerMode);
    buttonsTriggerMode->setText("Stop",0);
    buttonsTriggerMode->setColor(BACKGROUND_COLOR_ORANGE,0);
    buttonsTriggerMode->setText("Normal",1);
    buttonsTriggerMode->setText("Auto",2);

    WidgetButtons *buttonsTriggerChannel = new WidgetButtons(WidgetGeneral,4,ButtonTypes::RADIO,"Channel");
    verticalLayoutGeneral->addWidget(buttonsTriggerChannel);
    buttonsTriggerChannel->setText("CH1",0);
    buttonsTriggerChannel->setText("CH2",1);
    buttonsTriggerChannel->setText("CH3",2);
    buttonsTriggerChannel->setText("CH4",3);

    WidgetButtons *buttonsTriggerEdge = new WidgetButtons(WidgetGeneral,2,ButtonTypes::RADIO,"Edge");
    verticalLayoutGeneral->addWidget(buttonsTriggerEdge);
    buttonsTriggerEdge->setText("Rise",0);
    buttonsTriggerEdge->setText("Fall",1);

    WidgetDialRange *dialPretrigger = new WidgetDialRange(WidgetGeneral ,"Pretrigger");
    dialPretrigger->setRange(0,100,"%",1,1,50);
    dialPretrigger->hideUnitSelection();
    verticalLayoutGeneral->addWidget(dialPretrigger);

    WidgetDialRange *dialTriggerValue = new WidgetDialRange(WidgetGeneral ,"Level");
    dialTriggerValue->setRange(0,3.3,"V",10,0.01,1.65);
    dialTriggerValue->hideUnitSelection();
    verticalLayoutGeneral->addWidget(dialTriggerValue);



    WidgetSeparator *separatorHorizontal = new WidgetSeparator(WidgetGeneral,"Horizontal");
    verticalLayoutGeneral->addWidget(separatorHorizontal);

    dialTimeBase = new WidgetDial(WidgetGeneral ,"Time base");
    verticalLayoutGeneral->addWidget(dialTimeBase);
    fillTimeBase();
    dialTimeBase->setSelected(6);

    WidgetButtons *buttonsMemorySet = new WidgetButtons(WidgetGeneral,2,ButtonTypes::RADIO,"Memory");
    verticalLayoutGeneral->addWidget(buttonsMemorySet);
    buttonsMemorySet->setText("Normal",0);
    buttonsMemorySet->setText(" Long ",1);



    WidgetSeparator *separatorVertical = new WidgetSeparator(WidgetGeneral,"Vertical");
    verticalLayoutGeneral->addWidget(separatorVertical);

    WidgetButtons *buttonsChannelVerticalGain = new WidgetButtons(WidgetGeneral,4,ButtonTypes::RADIO);
    verticalLayoutGeneral->addWidget(buttonsChannelVerticalGain);
    buttonsChannelVerticalGain->setText("CH1",0);
    buttonsChannelVerticalGain->setText("CH2",1);
    buttonsChannelVerticalGain->setText("CH3",2);
    buttonsChannelVerticalGain->setText("CH4",3);

    WidgetDial *dialVerticalGain = new WidgetDial(WidgetGeneral ,"Scale");
    verticalLayoutGeneral->addWidget(dialVerticalGain);
    dialVerticalGain->addOption("10","mV/div",0.01);
    dialVerticalGain->addOption("20","mV/div",0.02);
    dialVerticalGain->addOption("50","mV/div",0.05);
    dialVerticalGain->addOption("100","mV/div",0.1);
    dialVerticalGain->addOption("200","mV/div",0.2);
    dialVerticalGain->addOption("500","mV/div",0.5);
    dialVerticalGain->addOption("1","V/div",1);
    dialVerticalGain->addOption("2","V/div",2);
    dialVerticalGain->setSelected(6);

    WidgetDialRange *dialVerticalShift = new WidgetDialRange(WidgetGeneral ,"Shift");
    dialVerticalShift->setRange(-3.3,6.6,"V",10,0.01,0);
    //dialVerticalShift->hideUnitSelection();
    verticalLayoutGeneral->addWidget(dialVerticalShift);



    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutGeneral->addItem(verticalSpacer);

    scrollAreaGeneral->show();


    //************************* creating widget general settings *******************
    //create scroll area - each seting selection has its own scroll area
    scrollAreaMeas = new QScrollArea(ui->widget_settings);
    scrollAreaMeas->setWidgetResizable(true);
    scrollAreaMeas->setSizePolicy(sizePolicy);

    WidgetMeas = new QWidget();
    WidgetMeas->setSizePolicy(sizePolicy);
    verticalLayoutMeas = new QVBoxLayout(WidgetMeas);

    scrollAreaMeas->setContentsMargins(0,0,0,0);
    verticalLayoutMeas->setSpacing(3);
    verticalLayoutMeas->setContentsMargins(10,0,10,0);

    scrollAreaMeas->setWidget(WidgetMeas);
    ui->verticalLayout_settings->addWidget(scrollAreaMeas);

    //add items

    WidgetSeparator *sepa = new WidgetSeparator(WidgetMeas,"Measurement");
    verticalLayoutMeas->addWidget(sepa);



    WidgetSeparator *separator = new WidgetSeparator(WidgetMeas,"Separator - just some stuff to test");
    verticalLayoutMeas->addWidget(separator);

    WidgetButtons *butrad = new WidgetButtons(WidgetMeas,4,ButtonTypes::RADIO,"Selection");
    verticalLayoutMeas->addWidget(butrad);
    butrad->setText("CH1",0);
    butrad->setColor(BACKGROUND_COLOR_ORANGE,0);
    butrad->setText("CH2",1);
    butrad->setColor(BACKGROUND_COLOR_BLUE,1);
    butrad->setText("CH3",2);
    butrad->setColor(BACKGROUND_COLOR_GREEN,2);
    butrad->setText("CH4",3);




    WidgetDialRange *valb = new WidgetDialRange(WidgetMeas ,"Freq");
    valb->setRange(1000,10000000,"Hz",1,1000,10000,true);
    verticalLayoutMeas->addWidget(valb);



    WidgetDialRange *vala = new WidgetDialRange(WidgetMeas ,"Voltage");
    vala->setRange(-3.3,3.3,"V",10,0.01);
    verticalLayoutMeas->addWidget(vala);


    WidgetSwitch *wsw = new WidgetSwitch(WidgetMeas,"prvni");
    verticalLayoutMeas->addWidget(wsw);

    WidgetLabel *wlb = new WidgetLabel(WidgetMeas, "Channel y enable");
    verticalLayoutMeas->addWidget(wlb);



    WidgetSelection *wsel = new WidgetSelection(WidgetMeas,"Gain");
    wsel->addOption("1x",1);
    wsel->addOption("2x",2);
    wsel->addOption("4x",4);
    verticalLayoutMeas->addWidget(wsel);

    WidgetButtons *but2 = new WidgetButtons(WidgetMeas,1,ButtonTypes::NORMAL,"Peak to peak");
    verticalLayoutMeas->addWidget(but2);
    but2->setText("  Add  ",0);

    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutMeas->addItem(verticalSpacer);

    scrollAreaMeas->hide();



    //*********************** end of widgets **************



}

WindowScope::~WindowScope()
{
    delete ui;
}


void WindowScope::generalSettingsClicked(){
    WidgetTopSelection->setSelected(0);
    scrollAreaGeneral->show();
    scrollAreaMeas->hide();


}
void WindowScope::generalMeasClicked(){
    WidgetTopSelection->setSelected(1);
    scrollAreaGeneral->hide();
    scrollAreaMeas->show();

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




