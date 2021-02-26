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
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH2", "Channel 2", styleSheet, true, 1, HISTORY_SIZE, this);
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH3", "Channel 3", styleSheet, true, 1, HISTORY_SIZE, this);
    displays.append(display);
    display  = new WidgetDisplay("Voltmeter_CH4", "Channel 4", styleSheet, true, 1, HISTORY_SIZE, this);
    displays.append(display);

    foreach(WidgetDisplay * dis, displays){
       dis->setContentsMargins(5, 5, 5, 5);
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

    WidgetSeparator *separatorAvg = new WidgetSeparator(this,"Averaging");
    tabs->getLayout(0)->addWidget(separatorAvg);

    dialAveraging = new WidgetDialRange(tabs ,"Samples");
    dialAveraging->setObjectName("voltAvgSamples");
    dialAveraging->setRange(1,64,"x",1,1,4,false);
    dialAveraging->hideUnitSelection();
    tabs->getLayout(0)->addWidget(dialAveraging);

    WidgetSeparator *separatorSet = new WidgetSeparator(this,"Channel settings");
    tabs->getLayout(0)->addWidget(separatorSet);

    buttonsChannelSettings = new WidgetButtons(this,4,ButtonTypes::RADIO);
    buttonsChannelSettings->setObjectName("Channelsettings");
    tabs->getLayout(0)->addWidget(buttonsChannelSettings);
    buttonsChannelSettings->setText("CH1",0);
    buttonsChannelSettings->setText("CH2",1);
    buttonsChannelSettings->setText("CH3",2);
    buttonsChannelSettings->setText("CH4",3);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(0)),0);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(1)),1);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(2)),2);
    buttonsChannelSettings->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(3)),3);

    voltageACDC = new WidgetButtons(this,2,ButtonTypes::RADIO,"Voltage",0);
    tabs->getLayout(0)->addWidget(voltageACDC);
    voltageACDC->setText("   DC   ",0);
    voltageACDC->setText("   AC   ",1);

    buttonsClac = new WidgetButtons(this,3,ButtonTypes::RADIO,"",0);
    buttonsClac->setObjectName("buttonscalc");
    tabs->getLayout(0)->addWidget(buttonsClac);
    buttonsClac->setText("Min/Max",0);
    buttonsClac->setText("Ripple",1);
    buttonsClac->setText("Reset",2);
    buttonsClac->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(12)),2);


    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    tabs->getLayout(0)->addItem(verticalSpacer);


    connect(buttonsChannelEnable,&WidgetButtons::statusChanged,this,&VoltmeterWindow::channelEnableCallback);
    connect(buttonsChannelSettings,&WidgetButtons::clicked,this,&VoltmeterWindow::channelSettingsCallback);
}

VoltmeterWindow::~VoltmeterWindow()
{
    delete ui;
}


void VoltmeterWindow::restoreGUIAfterStartup()
{
    channelEnableCallback(buttonsChannelEnable->getStatus());
    channelSettingsCallback(buttonsChannelSettings->getSelectedIndex());
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}

void VoltmeterWindow::channelEnableCallback(int status)
{
    for(int i =0;i<4;i++){
        if((int)(pow(2,i))&(status)){
            displays.at(i)->show();
        }else{
            displays.at(i)->hide();
        }
    }
}

void VoltmeterWindow::averagingCallback(int value)
{

}

void VoltmeterWindow::channelSettingsCallback(int clicked)
{
    buttonsClac->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),0);
    buttonsClac->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),1);

    voltageACDC->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),0);
    voltageACDC->setColor("background-color:"+QString::fromUtf8(Colors::getChannelColorString(clicked)),1);
}

void VoltmeterWindow::voltageCallback(int clicked)
{

}

void VoltmeterWindow::buttonsCalsCallback(int clicked)
{

}
