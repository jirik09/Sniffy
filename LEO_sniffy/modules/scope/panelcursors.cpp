#include "panelcursors.h"

PanelCursors::PanelCursors(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Cursors"));

    cursorTypeButtons = new WidgetButtons(parent,3,ButtonTypes::RADIO,"");
    cursorTypeButtons->setObjectName("cursortypeSel");
    destination->addWidget(cursorTypeButtons);
    cursorTypeButtons->setText("None",0);
    cursorTypeButtons->setColor(Graphics::palette().unused,0);
    cursorTypeButtons->setText("Horizontal",1);
    cursorTypeButtons->setText("Vertical",2);

    destination->addWidget(new WidgetSeparator(parent,""));


    channelButtons = new WidgetButtons(parent,5,ButtonTypes::RADIO,"");
    channelButtons->setObjectName("cursorChannSelection");
    destination->addWidget(channelButtons);
    channelButtons->setText("CH1",0);
    channelButtons->setColor(Graphics::getChannelColor(0),0);
    channelButtons->setText("CH2",1);
    channelButtons->setColor(Graphics::getChannelColor(1),1);
    channelButtons->setText("CH3",2);
    channelButtons->setColor(Graphics::getChannelColor(2),2);
    channelButtons->setText("CH4",3);
    channelButtons->setColor(Graphics::getChannelColor(3),3);
    channelButtons->setText("Math/FFT",4);
    channelButtons->setColor(Graphics::getChannelColor(4),4);
    channelButtons->setEnabled(false);

    cursorHorADial = new WidgetDialRange(parent ,"Cursor A");
    cursorHorADial->setObjectName("horCurA");
    cursorHorADial->setRange(-0.0001,0.0001,"s",0.1,0.0000001,0);
    cursorHorADial->setColor(Graphics::getChannelColor(0));
    cursorHorADial->hideUnitSelection();
    cursorHorADial->enableFineMousePrecision();
    destination->addWidget(cursorHorADial);
    cursorHorADial->hide();

    cursorHorBDial = new WidgetDialRange(parent ,"Cursor B");
    cursorHorBDial->setObjectName("horCurB");
    cursorHorBDial->setRange(-0.0001,0.0001,"s",0.1,0.0000001,0);
    cursorHorBDial->setColor(Graphics::getChannelColor(0));
    cursorHorBDial->hideUnitSelection();
    cursorHorBDial->enableFineMousePrecision();
    destination->addWidget(cursorHorBDial);
    cursorHorBDial->hide();

    cursorVerADial = new WidgetDialRange(parent ,"Cursor A");
    cursorVerADial->setObjectName("verCurA");
    cursorVerADial->setRange(-1,1,"V",10,0.01,0);
    cursorVerADial->setColor(Graphics::getChannelColor(0));
    cursorVerADial->hideUnitSelection();
    cursorVerADial->enableFineMousePrecision();
    destination->addWidget(cursorVerADial);
    cursorVerADial->hide();

    cursorVerBDial = new WidgetDialRange(parent ,"Cursor B");
    cursorVerBDial->setObjectName("verCurB");
    cursorVerBDial->setRange(-1,1,"V",10,0.01,0);
    cursorVerBDial->setColor(Graphics::getChannelColor(0));
    cursorVerBDial->hideUnitSelection();
    cursorVerBDial->enableFineMousePrecision();
    destination->addWidget(cursorVerBDial);
    cursorVerBDial->hide();

    cursorFFTHorADial = new WidgetDialRange(parent ,"Cursor A");
    cursorFFTHorADial->setObjectName("FFThorCurA");
    cursorFFTHorADial->setRange(01,1000,"Hz",1,1,0);
    cursorFFTHorADial->setColor(Graphics::getChannelColor(0));
    cursorFFTHorADial->hideUnitSelection();
    cursorFFTHorADial->enableFineMousePrecision();
    destination->addWidget(cursorFFTHorADial);
    cursorFFTHorADial->hide();

    cursorFFTHorBDial = new WidgetDialRange(parent ,"Cursor B");
    cursorFFTHorBDial->setObjectName("FFThorCurB");
    cursorFFTHorBDial->setRange(1,1000,"Hz",1,1,0);
    cursorFFTHorBDial->setColor(Graphics::getChannelColor(0));
    cursorFFTHorBDial->hideUnitSelection();
    cursorFFTHorBDial->enableFineMousePrecision();
    destination->addWidget(cursorFFTHorBDial);
    cursorFFTHorBDial->hide();

    cursorFFTVerADial = new WidgetDialRange(parent ,"Cursor A");
    cursorFFTVerADial->setObjectName("FFTverCurA");
    cursorFFTVerADial->setRange(-1,1,"V",10,0.01,0);
    cursorFFTVerADial->setColor(Graphics::getChannelColor(0));
    cursorFFTVerADial->hideUnitSelection();
    cursorFFTVerADial->enableFineMousePrecision();
    destination->addWidget(cursorFFTVerADial);
    cursorFFTVerADial->hide();

    cursorFFTVerBDial = new WidgetDialRange(parent ,"Cursor B");
    cursorFFTVerBDial->setObjectName("FFTverCurB");
    cursorFFTVerBDial->setRange(-1,1,"V",10,0.01,0);
    cursorFFTVerBDial->setColor(Graphics::getChannelColor(0));
    cursorFFTVerBDial->hideUnitSelection();
    cursorFFTVerBDial->enableFineMousePrecision();
    destination->addWidget(cursorFFTVerBDial);
    cursorFFTVerBDial->hide();


    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}

void PanelCursors::ValidatePanelGUI(int type)
{
    int channel = channelButtons->getSelectedIndex();
    if(type == 0){  //none
        channelButtons->setEnabled(false);
        cursorHorADial->hide();
        cursorHorBDial->hide();
        cursorVerADial->hide();
        cursorVerBDial->hide();
        cursorFFTHorADial->hide();
        cursorFFTHorBDial->hide();
        cursorFFTVerADial->hide();
        cursorFFTVerBDial->hide();

    }else if(type == 1){  //horizontal
        channelButtons->setEnabled(true);
        if(channelButtons->getSelectedIndex() == 4){
            cursorFFTHorADial->show();
            cursorFFTHorBDial->show();
            cursorFFTHorADial->setColor(Graphics::getChannelColor(channel));
            cursorFFTHorBDial->setColor(Graphics::getChannelColor(channel));
            cursorHorADial->hide();
            cursorHorBDial->hide();
        }else{
            cursorHorADial->show();
            cursorHorBDial->show();
            cursorHorADial->setColor(Graphics::getChannelColor(channel));
            cursorHorBDial->setColor(Graphics::getChannelColor(channel));
            cursorFFTHorADial->hide();
            cursorFFTHorBDial->hide();
        }
        cursorVerADial->hide();
        cursorVerBDial->hide();
        cursorFFTVerADial->hide();
        cursorFFTVerBDial->hide();

    }else if(type == 2){  //vertical
        channelButtons->setEnabled(true);
        cursorHorADial->hide();
        cursorHorBDial->hide();
        cursorFFTHorADial->hide();
        cursorFFTHorBDial->hide();
        if(channelButtons->getSelectedIndex() == 4){
            cursorVerADial->hide();
            cursorVerBDial->hide();
            cursorFFTVerADial->show();
            cursorFFTVerBDial->show();
            cursorFFTVerADial->setColor(Graphics::getChannelColor(channel));
            cursorFFTVerBDial->setColor(Graphics::getChannelColor(channel));
        }else{
            cursorFFTVerADial->hide();
            cursorFFTVerBDial->hide();
            cursorVerADial->show();
            cursorVerBDial->show();
            cursorVerADial->setColor(Graphics::getChannelColor(channel));
            cursorVerBDial->setColor(Graphics::getChannelColor(channel));
        }
    }
}
