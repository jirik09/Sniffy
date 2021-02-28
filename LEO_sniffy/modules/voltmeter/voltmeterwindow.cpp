#include "voltmeterwindow.h"
#include "ui_voltmeterwindow.h"

VoltmeterWindow::VoltmeterWindow(VoltmeterConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoltmeterWindow),
    config(config)
{
    ui->setupUi(this);


    //setup displays
    QString styleSheet = IMAGE_UNITS_VOLT;
    WidgetDisplay *display;

    display  = new WidgetDisplay("Voltmeter_CH1", "Channel 1", styleSheet, true, 1, HISTORY_SIZE, this);
    display->configLabel(0,"CH1","color:"+Colors::getChannelColorString(0),true);
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH2", "Channel 2", styleSheet, true, 1, HISTORY_SIZE, this);
    display->configLabel(0,"CH2","color:"+Colors::getChannelColorString(1),true);
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH3", "Channel 3", styleSheet, true, 1, HISTORY_SIZE, this);
    display->configLabel(0,"CH3","color:"+Colors::getChannelColorString(2),true);
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH4", "Channel 4", styleSheet, true, 1, HISTORY_SIZE, this);
    displays.append(display);
    display->configLabel(0,"CH4","color:"+Colors::getChannelColorString(3),true);

    foreach(WidgetDisplay * dis, displays){
        dis->setContentsMargins(5, 5, 5, 5);
        dis->showAvgDisplay(false);
        dis->configLabel(2,"VOLTAGE","color:"+QString::fromUtf8(COLOR_GREY),true);
        ui->verticalLayout_display->addWidget(dis);
    }

    //setup control
    tabs = new widgetTab(ui->widget_settings, 2);
    tabs->setText("Control", 0);
    tabs->setText("Data log", 1);
    ui->verticalLayout_settings->addWidget(tabs);

    WidgetSeparator *separatorChannelEnable = new WidgetSeparator(this,"Channel enable");
    tabs->getLayout(0)->addWidget(separatorChannelEnable);

    buttonsChannelEnable = new WidgetButtons(this,4,ButtonTypes::CHECKABLE);
    buttonsChannelEnable->setObjectName("ChannelEnable");
    tabs->getLayout(0)->addWidget(buttonsChannelEnable);
    buttonsChannelEnable->setText("CH1",0);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(0)),0);
    buttonsChannelEnable->setText("CH2",1);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(1)),1);
    buttonsChannelEnable->setText("CH3",2);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(2)),2);
    buttonsChannelEnable->setText("CH4",3);
    buttonsChannelEnable->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(3)),3);

    labelVdd = new WidgetLabel(this,"Measured Vcc","--V");
    tabs->getLayout(0)->addWidget(labelVdd);

    WidgetSeparator *separatorAvg = new WidgetSeparator(this,"Averaging");
    tabs->getLayout(0)->addWidget(separatorAvg);

    dialAveraging = new WidgetDialRange(tabs ,"Samples");
    dialAveraging->setObjectName("voltAvgSamples");
    dialAveraging->setRange(1,64,"x",1,1,4,false);
    dialAveraging->hideUnitSelection();
    tabs->getLayout(0)->addWidget(dialAveraging);

    labelProgress = new WidgetLabel(this,"Progress","-/-");
    tabs->getLayout(0)->addWidget(labelProgress);

    WidgetSeparator *separatorSet = new WidgetSeparator(this,"Display");
    tabs->getLayout(0)->addWidget(separatorSet);

  /*  buttonsChannelSettings = new WidgetButtons(this,4,ButtonTypes::RADIO);
    buttonsChannelSettings->setObjectName("Channelsettings");
    tabs->getLayout(0)->addWidget(buttonsChannelSettings);
    buttonsChannelSettings->setText("CH1",0);
    buttonsChannelSettings->setText("CH2",1);
    buttonsChannelSettings->setText("CH3",2);
    buttonsChannelSettings->setText("CH4",3);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(0)),0);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(1)),1);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(2)),2);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(3)),3);*/

   /* voltageACDC = new WidgetButtons(this,2,ButtonTypes::RADIO,"Voltage",0);
    tabs->getLayout(0)->addWidget(voltageACDC);
    voltageACDC->setText("   DC   ",0);
    voltageACDC->setText("   AC   ",1);
    voltageACDC->enableAll(false);*/

    buttonsCalc = new WidgetButtons(this,3,ButtonTypes::RADIO,"",0);
    buttonsCalc->setObjectName("buttonscalc");
    tabs->getLayout(0)->addWidget(buttonsCalc);
    buttonsCalc->setText("Min/Max",0);
    buttonsCalc->setText("Ripple",1);
    buttonsCalc->setText("None",2);
    buttonsCalc->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(12)),2);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    tabs->getLayout(0)->addItem(verticalSpacer);


    connect(buttonsChannelEnable,&WidgetButtons::statusChanged,this,&VoltmeterWindow::channelEnableCallback);
    //connect(buttonsChannelSettings,&WidgetButtons::clicked,this,&VoltmeterWindow::channelSettingsCallback);
    connect(dialAveraging,&WidgetDialRange::valueChanged,this,&VoltmeterWindow::averagingCallback);
    connect(buttonsCalc,&WidgetButtons::statusChanged,this,&VoltmeterWindow::buttonsCalcsCallback);
}

VoltmeterWindow::~VoltmeterWindow()
{
    delete ui;
}


void VoltmeterWindow::restoreGUIAfterStartup()
{
    channelEnableCallback(buttonsChannelEnable->getStatus());
    //channelSettingsCallback(buttonsChannelSettings->getSelectedIndex());
    averagingCallback(dialAveraging->getRealValue());
    showEmptyCalcs();
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}

void VoltmeterWindow::showData(ChannelData data[], int numChannels){
    for(int i = 0;i<numChannels;i++){
        displays.at(i)->displayString(displays.at(i)->formatNumber(data[i].voltage,'f',4));
        displays.at(i)->updateProgressBar(data[i].percent);
        displays.at(i)->appendNewHistorySample("",data[i].voltage , "V", 1);


        if(buttonsCalc->getSelectedIndex()==0){
            displays.at(i)->displayQerrString(displays.at(i)->formatNumber(data[i].min,'f',4));
            displays.at(i)->displayTerrString(displays.at(i)->formatNumber(data[i].max,'f',4));
            QString styleSheet = IMAGE_UNITS_VMAX;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_VMIN;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else if(buttonsCalc->getSelectedIndex()==1){
            displays.at(i)->displayQerrString(displays.at(i)->formatNumber(data[i].frequency,'f',4));
            displays.at(i)->displayTerrString(displays.at(i)->formatNumber(data[i].ripple,'f',4));
            QString styleSheet = IMAGE_UNITS_VRIPPLE;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_HZ;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else{
            displays[i]->showErrDisplay(false);
            displays[i]->showTerrStyle(false);
            displays.at(i)->displayQerrString("");
            displays.at(i)->displayTerrString("");
        }
    }
}

void VoltmeterWindow::showEmptyCalcs(){
    for(int i = 0;i<MAX_VOLTMETER_CHANNELS;i++){
        if(buttonsCalc->getSelectedIndex()==0){
            displays.at(i)->displayQerrString("--");
            displays.at(i)->displayTerrString("--");
            QString styleSheet = IMAGE_UNITS_VMAX;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_VMIN;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else if(buttonsCalc->getSelectedIndex()==1){
            displays.at(i)->displayQerrString("--");
            displays.at(i)->displayTerrString("--");
            QString styleSheet = IMAGE_UNITS_VRIPPLE;
            displays[i]->setErrStyle(styleSheet);
            styleSheet = IMAGE_UNITS_HZ;
            displays[i]->setTerrStyle(styleSheet);
            displays[i]->showErrDisplay(true);
            displays[i]->showTerrStyle(true);
        }else{
            displays[i]->showErrDisplay(false);
            displays[i]->showTerrStyle(false);
            displays.at(i)->displayQerrString("");
            displays.at(i)->displayTerrString("");
        }
    }
}

void VoltmeterWindow::showVddValue(qreal value){
    labelVdd->setValue(QString::number(value)+" V");
}

void VoltmeterWindow::showProgress(int current, int max){
    labelProgress->setValue(QString::number(current)+"/"+QString::number(max));
}

void VoltmeterWindow::setPins(QString pins[], int numOfCh)
{
    for(int i = 0;i<numOfCh;i++){
        displays.at(i)->configLabel(1,"pin "+pins[i],"color:"+QString::fromUtf8(COLOR_GREY),true);
    }
}

void VoltmeterWindow::channelEnableCallback(int status)
{
    int numChEnabled=0;
    for(int i =0;i<4;i++){
        if((int)(pow(2,i))&(status)){
            displays.at(i)->show();
            numChEnabled = i+1;
        }else{
            displays.at(i)->hide();
        }
    }
    emit numChannelsEnabledChanged(numChEnabled);
}

void VoltmeterWindow::averagingCallback(int value)
{
    emit averagingChanged(value);
}

void VoltmeterWindow::channelSettingsCallback(int clicked)
{
  /*  buttonsCalc->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),0);
    buttonsCalc->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),1);

    voltageACDC->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),0);
    voltageACDC->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),1);*/
}

void VoltmeterWindow::voltageCallback(int clicked)
{

}

void VoltmeterWindow::buttonsCalcsCallback(int clicked)
{
    showEmptyCalcs();
    if(clicked!=0){
        emit resetMinMax();
    }
}
