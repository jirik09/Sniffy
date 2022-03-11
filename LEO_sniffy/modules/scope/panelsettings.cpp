#include "panelsettings.h"

PanelSettings::PanelSettings(QVBoxLayout *destination, QWidget *parent ) : QObject(parent)
{
    //add items - only specified widgets like buttons, dials, labels, separators....
    WidgetSeparator *separatorChannelEnable = new WidgetSeparator(parent,"Channel enable");
    destination->addWidget(separatorChannelEnable);

    buttonsChannelEnable = new WidgetButtons(parent,4,ButtonTypes::CHECKABLE);
    buttonsChannelEnable->setObjectName("ChannelEnable");
    destination->addWidget(buttonsChannelEnable);
    buttonsChannelEnable->setText("CH1",0);
    buttonsChannelEnable->setColor(Graphics::getChannelColor(0),0);
    buttonsChannelEnable->setText("CH2",1);
    buttonsChannelEnable->setColor(Graphics::getChannelColor(1),1);
    buttonsChannelEnable->setText("CH3",2);
    buttonsChannelEnable->setColor(Graphics::getChannelColor(2),2);
    buttonsChannelEnable->setText("CH4",3);
    buttonsChannelEnable->setColor(Graphics::getChannelColor(3),3);

    WidgetSeparator *separatorTrig = new WidgetSeparator(parent,"Trigger");
    destination->addWidget(separatorTrig);

    buttonsTriggerMode = new WidgetButtons(parent,3,ButtonTypes::RADIO,"",2);
    buttonsTriggerMode->setObjectName("TriggerMode");
    destination->addWidget(buttonsTriggerMode);
    buttonsTriggerMode->setText("Stop",0);
    buttonsTriggerMode->setColor(Graphics::COLOR_WARNING,0);
    buttonsTriggerMode->setText("Normal",1);
    buttonsTriggerMode->setText("Auto",2);

    buttonsTriggerChannel = new WidgetButtons(parent,4,ButtonTypes::RADIO,"Channel");
    buttonsTriggerChannel->setObjectName("TriggerChannel");
    destination->addWidget(buttonsTriggerChannel);
    buttonsTriggerChannel->setText(" CH1 ",0);
    buttonsTriggerChannel->setText(" CH2 ",1);
    buttonsTriggerChannel->setText(" CH3 ",2);
    buttonsTriggerChannel->setText(" CH4 ",3);
    buttonsTriggerChannel->setColor(Graphics::getChannelColor(0),0);
    buttonsTriggerChannel->setColor(Graphics::getChannelColor(1),1);
    buttonsTriggerChannel->setColor(Graphics::getChannelColor(2),2);
    buttonsTriggerChannel->setColor(Graphics::getChannelColor(3),3);

    buttonsTriggerChannel->disableAll();
    buttonsTriggerChannel->setDisabledButton(false,0);

    buttonsTriggerEdge = new WidgetButtons(parent,2,ButtonTypes::RADIO,"Edge");
    buttonsTriggerEdge->setObjectName("TriggerEdge");
    destination->addWidget(buttonsTriggerEdge);
    buttonsTriggerEdge->setText("Rise",0);
    buttonsTriggerEdge->setText("Fall",1);

    dialPretrigger = new WidgetDialRange(parent ,"Pretrigger");
    dialPretrigger->setObjectName("Pretrigger");
    dialPretrigger->setRange(0,100,"%",1,1,50);
    dialPretrigger->hideUnitSelection();
    destination->addWidget(dialPretrigger);

    dialTriggerValue = new WidgetDialRange(parent ,"Level");
    dialTriggerValue->setObjectName("TriggerValue");
    dialTriggerValue->setRange(0,3.3,"V",10,0.01,1.65);
    dialTriggerValue->hideUnitSelection();
    destination->addWidget(dialTriggerValue);

    WidgetSeparator *separatorHorizontal = new WidgetSeparator(parent,"Horizontal");
    destination->addWidget(separatorHorizontal);

    dialTimeBase = new WidgetDial(parent ,"Time base");
    dialTimeBase->setObjectName("TimeBase");
    destination->addWidget(dialTimeBase);
    fillTimeBase();
    dialTimeBase->setDefaultIndex(12);

    buttonsMemorySet = new WidgetButtons(parent,3,ButtonTypes::RADIO,"Memory");
    buttonsMemorySet->setObjectName("MemorySet");
    destination->addWidget(buttonsMemorySet);
    buttonsMemorySet->setText(" Normal ",0);
    buttonsMemorySet->setText(" Long ",1);
    buttonsMemorySet->setText(" Zoom ",2);

    WidgetSeparator *separatorVertical = new WidgetSeparator(parent,"Vertical");
    destination->addWidget(separatorVertical);

    buttonsChannelVertical = new WidgetButtons(parent,5,ButtonTypes::RADIO);
    buttonsChannelVertical->setObjectName("ChannelVertival");
    destination->addWidget(buttonsChannelVertical);
    buttonsChannelVertical->setText("CH1",0);
    buttonsChannelVertical->setText("CH2",1);
    buttonsChannelVertical->setText("CH3",2);
    buttonsChannelVertical->setText("CH4",3);
    buttonsChannelVertical->setText("Math",4);
    buttonsChannelVertical->setColor(Graphics::getChannelColor(0),0);
    buttonsChannelVertical->setColor(Graphics::getChannelColor(1),1);
    buttonsChannelVertical->setColor(Graphics::getChannelColor(2),2);
    buttonsChannelVertical->setColor(Graphics::getChannelColor(3),3);
    buttonsChannelVertical->setColor(Graphics::getChannelColor(4),4);

    dialVerticalScale = new WidgetDial(parent ,"Scale");
    dialVerticalScale->setObjectName("VerticalScale");
    destination->addWidget(dialVerticalScale);
    dialVerticalScale->setColor(Graphics::getChannelColor(0));
    dialVerticalScale->addOption("10","V/div",10);
    dialVerticalScale->addOption("5","V/div",5);
    dialVerticalScale->addOption("2","V/div",2);
    dialVerticalScale->addOption("1","V/div",1);
    dialVerticalScale->addOption("500","mV/div",0.5);
    dialVerticalScale->addOption("200","mV/div",0.2);
    dialVerticalScale->addOption("100","mV/div",0.1);
    dialVerticalScale->addOption("50","mV/div",0.05);
    dialVerticalScale->addOption("20","mV/div",0.02);
    dialVerticalScale->addOption("10","mV/div",0.01);
    dialVerticalScale->setDefaultIndex(3);

    dialVerticalShift = new WidgetDialRange(parent ,"Shift");
    dialVerticalShift->setObjectName("VerticalShift");
    dialVerticalShift->setRange(-3.3,6.6,"V",10,0.01,0);
    dialVerticalShift->setColor(Graphics::getChannelColor(0));
    dialVerticalShift->hideUnitSelection();
    destination->addWidget(dialVerticalShift);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}


void PanelSettings::fillTimeBase(){
    dialTimeBase->addOption("10","s/div",10);
    dialTimeBase->addOption("5","s/div",5);
    dialTimeBase->addOption("2","s/div",2);
    dialTimeBase->addOption("1","s/div",1);
    dialTimeBase->addOption("500","ms/div",5e-1);
    dialTimeBase->addOption("200","ms/div",2e-1);
    dialTimeBase->addOption("100","ms/div",1e-1);
    dialTimeBase->addOption("50","ms/div",5e-2);
    dialTimeBase->addOption("20","ms/div",2e-2);
    dialTimeBase->addOption("10","ms/div",1e-2);
    dialTimeBase->addOption("5","ms/div",5e-3);
    dialTimeBase->addOption("2","ms/div",2e-3);
    dialTimeBase->addOption("1","ms/div",1e-3);
    dialTimeBase->addOption("500","us/div",5e-4);
    dialTimeBase->addOption("200","us/div",2e-4);
    dialTimeBase->addOption("100","us/div",1e-4);
    dialTimeBase->addOption("50","us/div",5e-5);
    dialTimeBase->addOption("20","us/div",2e-5);
    dialTimeBase->addOption("10","us/div",1e-5);
    dialTimeBase->addOption("5","us/div",5e-6);
    dialTimeBase->addOption("2","us/div",2e-6);
    dialTimeBase->addOption("1","us/div",1e-6);
    dialTimeBase->addOption("500","ns/div",5e-7);
}
