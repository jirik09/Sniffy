#include "arbgenpanelsettings.h"

ArbGenPanelSettings::ArbGenPanelSettings(QVBoxLayout *destination, QWidget *parent) :
    QObject(parent)
{

    WidgetSeparator *separatorCommon = new WidgetSeparator(parent, "Common");
    destination->addWidget(separatorCommon);

    buttonsGenerate = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"");
    buttonsGenerate->setObjectName("arbgenbtnen");
    buttonsGenerate->setText("Start");

    buttonsEnable = new WidgetButtons(parent,4,ButtonTypes::CHECKABLE,"Enable");
    buttonsEnable->setObjectName("arbGenBtnEnableChannel");
    buttonsEnable->setText("  CH 1  ",0);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(0)),0);
    buttonsEnable->setText("  CH 2  ",1);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(1)),1);
    buttonsEnable->setText("  CH 3  ",2);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(2)),2);
    buttonsEnable->setText("  CH 4  ",3);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(3)),3);

    buttonsMemory = new WidgetButtons(parent,3,ButtonTypes::RADIO,"Memory");
    buttonsMemory->setObjectName("arbgenbtnmem");
    buttonsMemory->setText(" Best fit ",0);
    buttonsMemory->setText(" Long ",1);
    buttonsMemory->setText(" Custom ",2);

    customLengthInput = new WidgetTextInput(parent,"","200",InputTextType::NUMBER);
    customLengthInput->setObjectName("arbgenmemleng");


    QHBoxLayout *commonButtons = new QHBoxLayout();
    destination->addWidget(buttonsGenerate);
    commonButtons->addWidget(buttonsMemory);
    commonButtons->addWidget(customLengthInput);
    destination->addLayout(commonButtons);
    destination->addWidget(buttonsEnable);

    QHBoxLayout *horBox = new QHBoxLayout();
    destination->addLayout(horBox);
    for(int i = 0; i < MAX_ARB_CHANNELS_NUM; i++){
        QString chNStr = QString::number(i + 1);

        verChannArea[i] = new QScrollArea(parent);
        verChannArea[i]->setWidgetResizable(true);
        verChannArea[i]->setMinimumWidth(150);
        horBox->addWidget(verChannArea[i]);
        QWidget *setScroll = new QWidget(parent);

        verChannArea[i]->setWidget(setScroll);
        QVBoxLayout *verChanBox = new QVBoxLayout();
        setScroll->setLayout(verChanBox);


        WidgetSeparator *separator = new WidgetSeparator(parent, "Channel " + chNStr);
        verChanBox->addWidget(separator);

        buttonsShape[i] = new WidgetButtons(parent,4,ButtonTypes::RADIO,"",0,i);
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
        signalShape[i] = SignalShape::SINE;

        dialFreqCh[i] = new WidgetDialRange(parent,"Frequency",i);
        dialFreqCh[i]->setObjectName("arbGenfreq"+chNStr);
        dialFreqCh[i]->setRange(0.1,10000000,"Hz",10,0.01,1000,true);
        dialFreqCh[i]->setColor(Colors::getChannelColorString(i));
        verChanBox->addWidget(dialFreqCh[i]);

        if(i!=0){
            swSyncWithCH1[i] = new WidgetSwitch(parent,"Off","On","Sync with CH1",i);
            swSyncWithCH1[i]->setObjectName("SwFreqSynch"+chNStr);
            swSyncWithCH1[i]->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(i)));
            verChanBox->addWidget(swSyncWithCH1[i]);
            connect(swSyncWithCH1[i],&WidgetSwitch::clicked,this,&ArbGenPanelSettings::syncWithCH1Callback);
            channelSyncWithCH1[i] = false;
        }


        dialOffsetCh[i] = new WidgetDialRange(parent ,"Offset",i);
        dialOffsetCh[i]->setObjectName("arbGenOffset" + chNStr);
        dialOffsetCh[i]->setRange(-3.3,6.6,"V",0.05,0.01,1.65);
        dialOffsetCh[i]->setColor(Colors::getChannelColorString(i));
        dialOffsetCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialOffsetCh[i]);

        dialAmplitudeCh[i] = new WidgetDialRange(parent ,"Amplitude",i);
        dialAmplitudeCh[i]->setObjectName("arbGenAmpl" + chNStr);
        dialAmplitudeCh[i]->setRange(-3.3,6.6,"V",0.05,0.01,1.65);
        dialAmplitudeCh[i]->setColor(Colors::getChannelColorString(i));
        dialAmplitudeCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialAmplitudeCh[i]);

        dialDutyCh[i] = new WidgetDialRange(parent ,"Duty",i);
        dialDutyCh[i]->setObjectName("arbGenDuty" + chNStr);
        dialDutyCh[i]->setRange(0,100,"%",5,1,50);
        dialDutyCh[i]->setColor(Colors::getChannelColorString(i));
        dialDutyCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialDutyCh[i]);

        dialPhaseCh[i] = new WidgetDialRange(parent ,"Phase",i);
        dialPhaseCh[i]->setObjectName("arbGenPhase" + chNStr);
        dialPhaseCh[i]->setRange(0,360,"°",5,1,0);
        dialPhaseCh[i]->setColor(Colors::getChannelColorString(i));
        dialPhaseCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialPhaseCh[i]);


        labelRealFreq[i] = new WidgetLabel(parent,"Frequency","NA");
        verChanBox->addWidget(labelRealFreq[i]);
        labelDataLength[i] = new WidgetLabel(parent,"Data length","NA");

        verChanBox->addWidget(labelDataLength[i]);

        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        verChanBox->addItem(verticalSpacer);

        connect(buttonsShape[i],&WidgetButtons::clicked,this,&ArbGenPanelSettings::buttonShapeCallback);
        connect(dialFreqCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalFrequencyCallback);
        connect(dialOffsetCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        connect(dialAmplitudeCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        connect(dialDutyCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        connect(dialPhaseCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        setChannelShown(i,false);
    }
    connect(buttonsEnable,&WidgetButtons::statusChanged,this,&ArbGenPanelSettings::buttonEnableChannelCallback);
    connect(buttonsMemory,&WidgetButtons::clicked,this,&ArbGenPanelSettings::memoryCallback);
    connect(customLengthInput,&WidgetTextInput::numberChanged,this,&ArbGenPanelSettings::customLenghtCallback);

    QSpacerItem *verticalSpacerW = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    horBox->addItem(verticalSpacerW);

}

void ArbGenPanelSettings::setChannelShown(int index, bool isShown)
{
    if(isShown){
        verChannArea[index]->show();
    }else{
        verChannArea[index]->hide();
    }
}

void ArbGenPanelSettings::restoreGUI()
{
    buttonEnableChannelCallback(buttonsEnable->getStatus());
    memoryCallback(buttonsMemory->getSelectedIndex());
    customLengthInput->processInput();
    customLenghtCallback(customLengthInput->getValue());

    for(int i =0;i<MAX_ARB_CHANNELS_NUM;i++){
        buttonShapeCallback(buttonsShape[i]->getSelectedIndex(),i);
        if(i>0 && swSyncWithCH1[i]->isCheckedRight()){
            syncWithCH1Callback(1,i);
        }
    }
}

void ArbGenPanelSettings::setMaxNumChannels(int numChannels)
{
    for(int i = numChannels ; i<MAX_ARB_CHANNELS_NUM ; i++){
        buttonsEnable->setButtonHidden(true,i);
    }
}

void ArbGenPanelSettings::setLabels(QString freq, QString length, int index)
{
    labelRealFreq[index]->setValue(freq);
    labelDataLength[index]->setValue(length);
}

void ArbGenPanelSettings::setFreqLabel(QString freq, int index)
{
    labelRealFreq[index]->setValue(freq);
}

void ArbGenPanelSettings::setCopyFreq(int fromCh, int toCh)
{
    qreal value = dialFreqCh[fromCh]->getRealValue();
    dialFreqCh[toCh]->setRealValue(value,true);
}

void ArbGenPanelSettings::buttonEnableChannelCallback(int status)
{
    int tmpchannelsEnabled = 0;
    for(int i =0;i<MAX_ARB_CHANNELS_NUM;i++){
        if((int)(pow(2,i))&(status)){
            setChannelShown(i,true);
            tmpchannelsEnabled++;
            channelEnabled[i] = true;
        }else{
            setChannelShown(i,false);
            channelEnabled[i] = false;
        }
    }
    numChannelsEnabled = tmpchannelsEnabled;
    signalChangedCallback();
}

void ArbGenPanelSettings::buttonShapeCallback(int clicked, int channel)
{
    switch (clicked) {
    case 0:
        signalShape[channel] = SignalShape::SINE;
        dialDutyCh[channel]->hide();
        break;
    case 1:
        signalShape[channel] = SignalShape::SAW;
        dialDutyCh[channel]->show();
        break;
    case 2:
        signalShape[channel] = SignalShape::RECT;
        dialDutyCh[channel]->show();
        break;
    case 3:
        signalShape[channel] = SignalShape::ARB;
        dialDutyCh[channel]->show();
        break;
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::syncWithCH1Callback(int clicked, int channel)
{
    if(clicked==0){
        channelSyncWithCH1[channel] = false;
    }else{
        channelSyncWithCH1[channel] = true;
        setCopyFreq(0,channel);
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::signalFrequencyCallback(qreal value, int channel)
{
    Q_UNUSED(value);
    if(channel==0){
        for(int i = 1;i<MAX_ARB_CHANNELS_NUM;i++){
            if(channelSyncWithCH1[i]) {
                setCopyFreq(0,i);
            }
        }
    }else{
        if(channelSyncWithCH1[channel]) {
            dialFreqCh[0]->setRealValue(value,false);
        }
    }
    signalChangedCallback();
}


void ArbGenPanelSettings::memoryCallback(int index)
{
    if(index==0){
        memorySet = MemoryLength::BEST_FIT;
        customLengthInput->hide();
    }else if(index==1){
        memorySet = MemoryLength::LONG;
        customLengthInput->hide();
    }else if(index==2){
        memorySet = MemoryLength::CUSTOM;
        customLengthInput->show();
        customLength = round(customLengthInput->getValue());
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::customLenghtCallback(qreal value)
{
    customLength = round(value);
    signalChangedCallback();
}

void ArbGenPanelSettings::signalChangedCallback()
{
    emit signalChanged();
}
