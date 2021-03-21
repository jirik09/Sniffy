#include "arbgenpanelsettings.h"

ArbGenPanelSettings::ArbGenPanelSettings(QVBoxLayout *destination, ArbGeneratorConfig *config, QWidget *parent) :
    QObject(parent),
    config(config)
{

    WidgetSeparator *separatorCommon = new WidgetSeparator(parent, "Common");
    destination->addWidget(separatorCommon);

    buttonsEnable = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"");
    buttonsEnable->setObjectName("arbgenbtnen");
    buttonsEnable->setText("Enable");

    buttonsMemory = new WidgetButtons(parent,3,ButtonTypes::RADIO,"Memory");
    buttonsMemory->setObjectName("arbgenbtnmem");
    buttonsMemory->setText(" Best fit ",0);
    buttonsMemory->setText("   Long   ",1);
    buttonsMemory->setText(" Custom ",2);

    customLengthInput = new WidgetTextInput(parent,"","200",InputTextType::NUMBER);
    customLengthInput->setObjectName("arbgenmemleng");


    QHBoxLayout *commonButtons = new QHBoxLayout();
    destination->addWidget(buttonsEnable);
    destination->addLayout(commonButtons);

    commonButtons->addWidget(buttonsMemory);
    commonButtons->addWidget(customLengthInput);


    QHBoxLayout *horBox = new QHBoxLayout();
    destination->addLayout(horBox);
    for(int i = 0; i < MAX_ARB_CHANNELS_NUM; i++){
        QString chNStr = QString::number(i + 1);

        //QScrollArea *verChannArea = new QScrollArea();
        QVBoxLayout *verChanBox = new QVBoxLayout();
        //verChannArea->setLayout(verChanBox);
        horBox->addLayout(verChanBox);

        WidgetSeparator *separator = new WidgetSeparator(parent, "Channel " + chNStr);
        verChanBox->addWidget(separator);

        buttonsShape[i] = new WidgetButtons(parent,4,ButtonTypes::RADIO);
        buttonsShape[i]->setObjectName("arbGenBtnshape"+chNStr);

        buttonsShape[i]->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(i)),0);
        buttonsShape[i]->setText("Sine",0);
        buttonsShape[i]->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(i)),1);
        buttonsShape[i]->setText("Saw",1);
        buttonsShape[i]->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(i)),2);
        buttonsShape[i]->setText("Rect",2);
        buttonsShape[i]->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(i)),3);
        buttonsShape[i]->setText("Arb",3);
        verChanBox->addWidget(buttonsShape[i]);

        dialFreqCh[i] = new WidgetDialRange(parent,"Frequency");
        dialFreqCh[i]->setObjectName("arbGenfreq"+chNStr);
        dialFreqCh[i]->setRange(0.1,10000000,"Hz",10,0.1,1000,true);
        dialFreqCh[i]->setColor(Colors::getChannelColorString(i));
        verChanBox->addWidget(dialFreqCh[i]);


        dialOffsetCh[i] = new WidgetDialRange(parent ,"Offset");
        dialOffsetCh[i]->setObjectName("arbGenOffset" + chNStr);
        dialOffsetCh[i]->setRange(-3.3,6.6,"V",0.05,0.01,1.65);
        dialOffsetCh[i]->setColor(Colors::getChannelColorString(i));
        dialOffsetCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialOffsetCh[i]);

        dialAmplitudeCh[i] = new WidgetDialRange(parent ,"Amplitude");
        dialAmplitudeCh[i]->setObjectName("arbGenAmpl" + chNStr);
        dialAmplitudeCh[i]->setRange(-3.3,6.6,"V",0.05,0.01,1.65);
        dialAmplitudeCh[i]->setColor(Colors::getChannelColorString(i));
        dialAmplitudeCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialAmplitudeCh[i]);

        dialDutyCh[i] = new WidgetDialRange(parent ,"Duty");
        dialDutyCh[i]->setObjectName("arbGenDuty" + chNStr);
        dialDutyCh[i]->setRange(0,100,"%",5,1,50);
        dialDutyCh[i]->setColor(Colors::getChannelColorString(i));
        dialDutyCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialDutyCh[i]);

        dialPhaseCh[i] = new WidgetDialRange(parent ,"Phase");
        dialPhaseCh[i]->setObjectName("arbGenPhase" + chNStr);
        dialPhaseCh[i]->setRange(0,360,"°",5,1,0);
        dialPhaseCh[i]->setColor(Colors::getChannelColorString(i));
        dialPhaseCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialPhaseCh[i]);

        /*
    WidgetDialRange *dialFreqCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialOffsetCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialAmplitudeCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialDutyCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialPhaseCh[MAX_ARB_CHANNELS_NUM];
    WidgetSwitch *syncwithCh1[MAX_ARB_CHANNELS_NUM];
*/


        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        destination->addItem(verticalSpacer);



    }






  //  QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  //  destination->addItem(verticalSpacer);




    /*
    //common
    WidgetButtons *buttonsEnable;
    WidgetButtons *buttonsMemory;

    //for each channel
    WidgetButtons *buttonsShape[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialFreqCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialOffsetCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialAmplitudeCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialDutyCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialPhaseCh[MAX_ARB_CHANNELS_NUM];
    WidgetSwitch *syncwithCh1[MAX_ARB_CHANNELS_NUM];
*/

}
