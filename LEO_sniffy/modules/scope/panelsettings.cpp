#include "panelsettings.h"

PanelSettings::PanelSettings(QVBoxLayout *destination, QWidget *parent ) : QObject(parent)
{
    //add items - only specified widgets like buttons, dials, labels, separators....
    WidgetSeparator *separatorChannelEnable = new WidgetSeparator(parent,"Channel enable");
    destination->addWidget(separatorChannelEnable);

    buttonsChannelEnable = new WidgetButtons(parent,4,ButtonTypes::CHECKABLE);
    destination->addWidget(buttonsChannelEnable);
    buttonsChannelEnable->setText("CH1",0);
    buttonsChannelEnable->setColor(BCKGRND_COLOR_ORANGE,0);
    buttonsChannelEnable->setText("CH2",1);
    buttonsChannelEnable->setColor(BCKGRND_COLOR_BLUE,1);
    buttonsChannelEnable->setText("CH3",2);
    buttonsChannelEnable->setColor(BCKGRND_COLOR_GREEN,2);
    buttonsChannelEnable->setText("CH4",3);
    buttonsChannelEnable->setColor(BCKGRND_COLOR_PURPLE,3);

    WidgetSeparator *separatorTrig = new WidgetSeparator(parent,"Trigger");
    destination->addWidget(separatorTrig);

    buttonsTriggerMode = new WidgetButtons(parent,3,ButtonTypes::RADIO,"",2);
    destination->addWidget(buttonsTriggerMode);
    buttonsTriggerMode->setText("Stop",0);
    buttonsTriggerMode->setColor(BCKGRND_COLOR_ORANGE,0);
    buttonsTriggerMode->setText("Normal",1);
    buttonsTriggerMode->setText("Auto",2);

    buttonsTriggerChannel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"Channel");
    destination->addWidget(buttonsTriggerChannel);
    buttonsTriggerChannel->setText("CH1",0);
    buttonsTriggerChannel->setText("CH2",1);
    buttonsTriggerChannel->setText("CH3",2);
    buttonsTriggerChannel->setText("CH4",3);
    buttonsTriggerChannel->setColor(BCKGRND_COLOR_ORANGE,0);
    buttonsTriggerChannel->setColor(BCKGRND_COLOR_BLUE,1);
    buttonsTriggerChannel->setColor(BCKGRND_COLOR_GREEN,2);
    buttonsTriggerChannel->setColor(BCKGRND_COLOR_PURPLE,3);

    buttonsTriggerChannel->disableAll();
    buttonsTriggerChannel->setDisabledButton(false,0);

    buttonsTriggerEdge = new WidgetButtons(parent,2,ButtonTypes::RADIO,"Edge");
    destination->addWidget(buttonsTriggerEdge);
    buttonsTriggerEdge->setText("Rise",0);
    buttonsTriggerEdge->setText("Fall",1);

    dialPretrigger = new WidgetDialRange(parent ,"Pretrigger");
    dialPretrigger->setRange(0,100,"%",1,1,50);
    dialPretrigger->hideUnitSelection();
    destination->addWidget(dialPretrigger);

    dialTriggerValue = new WidgetDialRange(parent ,"Level");
    dialTriggerValue->setRange(0,3.3,"V",10,0.01,1.65);
    dialTriggerValue->hideUnitSelection();
    destination->addWidget(dialTriggerValue);

    WidgetSeparator *separatorHorizontal = new WidgetSeparator(parent,"Horizontal");
    destination->addWidget(separatorHorizontal);

    dialTimeBase = new WidgetDial(parent ,"Time base");
    destination->addWidget(dialTimeBase);
    fillTimeBase();
    dialTimeBase->setSelected(9);

    WidgetButtons *buttonsMemorySet = new WidgetButtons(parent,2,ButtonTypes::RADIO,"Memory");
    destination->addWidget(buttonsMemorySet);
    buttonsMemorySet->setText("Normal",0);
    buttonsMemorySet->setText(" Long ",1);

    WidgetSeparator *separatorVertical = new WidgetSeparator(parent,"Vertical");
    destination->addWidget(separatorVertical);

    WidgetButtons *buttonsChannelVerticalGain = new WidgetButtons(parent,4,ButtonTypes::RADIO);
    destination->addWidget(buttonsChannelVerticalGain);
    buttonsChannelVerticalGain->setText("CH1",0);
    buttonsChannelVerticalGain->setText("CH2",1);
    buttonsChannelVerticalGain->setText("CH3",2);
    buttonsChannelVerticalGain->setText("CH4",3);

    WidgetDial *dialVerticalGain = new WidgetDial(parent ,"Scale");
    destination->addWidget(dialVerticalGain);
    dialVerticalGain->addOption("10","mV/div",0.01);
    dialVerticalGain->addOption("20","mV/div",0.02);
    dialVerticalGain->addOption("50","mV/div",0.05);
    dialVerticalGain->addOption("100","mV/div",0.1);
    dialVerticalGain->addOption("200","mV/div",0.2);
    dialVerticalGain->addOption("500","mV/div",0.5);
    dialVerticalGain->addOption("1","V/div",1);
    dialVerticalGain->addOption("2","V/div",2);
    dialVerticalGain->setSelected(6);

    WidgetDialRange *dialVerticalShift = new WidgetDialRange(parent ,"Shift");
    dialVerticalShift->setRange(-3.3,6.6,"V",10,0.01,0);
    destination->addWidget(dialVerticalShift);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}



void PanelSettings::fillTimeBase(){
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
