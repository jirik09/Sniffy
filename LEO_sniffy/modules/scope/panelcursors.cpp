#include "panelcursors.h"

PanelCursors::PanelCursors(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Cursors"));

    cursorTypeButtons = new WidgetButtons(parent,3,ButtonTypes::RADIO,"");
    cursorTypeButtons->setObjectName("cursortypeSel");
    destination->addWidget(cursorTypeButtons);
    cursorTypeButtons->setText("None",0);
    cursorTypeButtons->setColor(QString::fromUtf8("background-color:")+COLOR_GREY,0);
    cursorTypeButtons->setText("Horizontal",1);
    cursorTypeButtons->setText("Vertical",2);

    destination->addWidget(new WidgetSeparator(parent,""));


    channelButtons = new WidgetButtons(parent,4,ButtonTypes::RADIO,"");
    channelButtons->setObjectName("cursorChannSelection");
    destination->addWidget(channelButtons);
    channelButtons->setText("CH1",0);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(0)),0);
    channelButtons->setText("CH2",1);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(1)),1);
    channelButtons->setText("CH3",2);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(2)),2);
    channelButtons->setText("CH4",3);
    channelButtons->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(3)),3);
    channelButtons->setEnabled(false);

    cursorHorADial = new WidgetDialRange(parent ,"Cursor A");
    cursorHorADial->setObjectName("horCurA");
    cursorHorADial->setRange(-0.0001,0.0001,"s",0.1,0.000000001,0);
    cursorHorADial->setColor(Colors::getChannelColorString(0));
    cursorHorADial->hideUnitSelection();
    cursorHorADial->enableFineMousePrecision();
    destination->addWidget(cursorHorADial);
    cursorHorADial->hide();

    cursorHorBDial = new WidgetDialRange(parent ,"Cursor B");
    cursorHorADial->setObjectName("horCurB");
    cursorHorBDial->setRange(-0.0001,0.0001,"s",0.1,0.000000001,0);
    cursorHorBDial->setColor(Colors::getChannelColorString(0));
    cursorHorBDial->hideUnitSelection();
    cursorHorBDial->enableFineMousePrecision();
    destination->addWidget(cursorHorBDial);
    cursorHorBDial->hide();

    cursorVerADial = new WidgetDialRange(parent ,"Cursor A");
    cursorHorADial->setObjectName("verCurA");
    cursorVerADial->setRange(-1,1,"V",10,0.01,0);
    cursorVerADial->setColor(Colors::getChannelColorString(0));
    cursorVerADial->hideUnitSelection();
    cursorVerADial->enableFineMousePrecision();
    destination->addWidget(cursorVerADial);
    cursorVerADial->hide();

    cursorVerBDial = new WidgetDialRange(parent ,"Cursor B");
    cursorHorADial->setObjectName("verCurB");
    cursorVerBDial->setRange(-1,1,"V",10,0.01,0);
    cursorVerBDial->setColor(Colors::getChannelColorString(0));
    cursorVerBDial->hideUnitSelection();
    cursorVerBDial->enableFineMousePrecision();
    destination->addWidget(cursorVerBDial);
    cursorVerBDial->hide();


    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}