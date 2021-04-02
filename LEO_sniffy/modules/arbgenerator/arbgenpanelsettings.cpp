#include "arbgenpanelsettings.h"

ArbGenPanelSettings::ArbGenPanelSettings(QVBoxLayout *destination, bool isPWMbased, QWidget *parent) :
    QObject(parent),
    isPWMbased(isPWMbased)
{

    WidgetSeparator *separatorCommon = new WidgetSeparator(parent, "Common");
    destination->addWidget(separatorCommon);

    buttonsGenerate = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"");
    buttonsGenerate->setObjectName("arbgenbtnen");
    buttonsGenerate->setText("Start");

    buttonsEnable = new WidgetButtons(parent,4,ButtonTypes::RADIO,"Number of channels");
    buttonsEnable->setObjectName("arbGenBtnEnableChannel");
    buttonsEnable->setText("  One  ",0);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(0)),0);
    buttonsEnable->setText("  Two  ",1);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(1)),1);
    buttonsEnable->setText("  Three  ",2);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(2)),2);
    buttonsEnable->setText("  Four  ",3);
    buttonsEnable->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(3)),3);

    buttonsMemory = new WidgetButtons(parent,3,ButtonTypes::RADIO,"Memory");
    buttonsMemory->setObjectName("arbgenbtnmem");
    buttonsMemory->setText(" Best fit ",0);
    buttonsMemory->setText(" Long ",1);
    buttonsMemory->setText(" Custom ",2);

    customLengthInput = new WidgetTextInput(parent,"","200",InputTextType::NUMBER);
    customLengthInput->setObjectName("arbgenmemleng");

    buttonSelectFile = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"     Arbitrary data");
    buttonSelectFile->setObjectName("buttonselectfileArbGen");
    buttonSelectFile->setText("   Select   ");

    labelArbFileInfo = new WidgetLabel(parent,"No arbitrary data file selected","");
    labelArbFileInfo->setObjectName("labelInfoArbGen");

    buttonSWSweepEnable = new WidgetButtons (parent,2,ButtonTypes::RADIO,"SW sweep CH1",0);
    buttonSWSweepEnable->setText("   Off   ",0);
    buttonSWSweepEnable->setText("   On   ",1);
    buttonSWSweepEnable->setColor(QString::fromUtf8("background-color:")+COLOR_GREY,0);
    buttonSWSweepEnable->setColor(QString::fromUtf8("background-color:")+COLOR_GREY,1);
    buttonSWSweepEnable->setObjectName("arbGenSweepbtn");

    QHBoxLayout *commonButtons = new QHBoxLayout();
    QHBoxLayout *commonButtons2 = new QHBoxLayout();
    destination->addWidget(buttonsGenerate);
    commonButtons->addWidget(buttonsMemory);
    commonButtons->addWidget(customLengthInput);
    destination->addLayout(commonButtons);
    destination->addWidget(buttonsEnable);
    commonButtons2->addWidget(buttonSelectFile);
    commonButtons2->addWidget(buttonSWSweepEnable);
    destination->addLayout(commonButtons2);
    destination->addWidget(labelArbFileInfo);


    sweepControl = new QHBoxLayout();
    dialFreqSweepMin = new WidgetDialRange(parent,"Min frequency");
    dialFreqSweepMin->setObjectName("arbGenfreqSweepMin");
    dialFreqSweepMin->setRange(0.1,1000000,"Hz",10,0.01,1000,true);
    dialFreqSweepMin->setColor(COLOR_GREY);

    dialFreqSweepMax = new WidgetDialRange(parent,"Max frequency");
    dialFreqSweepMax->setObjectName("arbGenfreqSweepMax");
    dialFreqSweepMax->setRange(0.1,1000000,"Hz",10,0.01,1000,true);
    dialFreqSweepMax->setColor(COLOR_GREY);

    dialFreqSweepTime = new WidgetDialRange(parent,"Sweep time");
    dialFreqSweepTime->setObjectName("arbGenfreqSweeptime");
    dialFreqSweepTime->setRange(0.1,60,"s",10,0.1,1,true);
    dialFreqSweepTime->setColor(COLOR_GREY);

    sweepControl->addWidget(dialFreqSweepMin);
    sweepControl->addWidget(dialFreqSweepMax);
    sweepControl->addWidget(dialFreqSweepTime);
    destination->addLayout(sweepControl);

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
        dialFreqCh[i]->setRange(0.1,1000000,"Hz",10,0.01,1000,true);
        dialFreqCh[i]->setColor(Colors::getChannelColorString(i));
        verChanBox->addWidget(dialFreqCh[i]);

        if(i!=0){
            swSyncWithCH1[i] = new WidgetSwitch(parent,"Off","On","CH1 Freq sync",i);
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
        dialPhaseCh[i]->setRealValue(i*90,true);
        dialPhaseCh[i]->setColor(Colors::getChannelColorString(i));
        dialPhaseCh[i]->hideUnitSelection();
        verChanBox->addWidget(dialPhaseCh[i]);


        labelRealFreq[i] = new WidgetLabel(parent,"Frequency","NA");
        verChanBox->addWidget(labelRealFreq[i]);
        labelDataLength[i] = new WidgetLabel(parent,"Data length","NA");

        verChanBox->addWidget(labelDataLength[i]);

        dialPWMFreqCh[i] = new WidgetDialRange(parent,"PWM Frequency",i);
        dialPWMFreqCh[i]->setObjectName("arbGenpwmfreq"+chNStr);
        dialPWMFreqCh[i]->setRange(1,10000000,"Hz",10,0.01,1000,true);
        dialPWMFreqCh[i]->setColor(Colors::getChannelColorString(i));
        verChanBox->addWidget(dialPWMFreqCh[i]);

        if(i!=0){
            swSyncPWMWithCH1[i] = new WidgetSwitch(parent,"Off","On","CH1 PWM sync",i);
            swSyncPWMWithCH1[i]->setObjectName("SwPWMFreqSynch"+chNStr);
            swSyncPWMWithCH1[i]->setColor(QString::fromUtf8("background-color:"+Colors::getChannelColorString(i)));
            verChanBox->addWidget(swSyncPWMWithCH1[i]);

            channelSyncPWMWithCH1[i] = false;
            if(!isPWMbased){
                swSyncPWMWithCH1[i]->hide();
            }else{
                connect(swSyncPWMWithCH1[i],&WidgetSwitch::clicked,this,&ArbGenPanelSettings::syncPWMWithCH1Callback);
            }
        }

        labelRealPWMFreq[i] = new WidgetLabel(parent,"PWM Frequency","NA");
        verChanBox->addWidget(labelRealPWMFreq[i]);

        labelPWMResolution[i] = new WidgetLabel(parent,"PWM Resolution","NA");
        verChanBox->addWidget(labelPWMResolution[i]);

        if(!isPWMbased){
            dialPWMFreqCh[i]->hide();
            labelRealPWMFreq[i]->hide();
            labelPWMResolution[i]->hide();
        }

        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        verChanBox->addItem(verticalSpacer);

        connect(buttonsShape[i],&WidgetButtons::clicked,this,&ArbGenPanelSettings::buttonShapeCallback);
        connect(dialFreqCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalFrequencyCallback);
        connect(dialOffsetCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        connect(dialAmplitudeCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        connect(dialDutyCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        connect(dialPhaseCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalChangedCallback);
        if(isPWMbased){
            connect(dialPWMFreqCh[i],&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::signalPWMFrequencyCallback);
        }
        setChannelShown(i,false);
    }
    connect(buttonsEnable,&WidgetButtons::clicked,this,&ArbGenPanelSettings::buttonEnableChannelCallback);
    connect(buttonsMemory,&WidgetButtons::clicked,this,&ArbGenPanelSettings::memoryCallback);
    connect(customLengthInput,&WidgetTextInput::numberChanged,this,&ArbGenPanelSettings::customLenghtCallback);
    connect(buttonSWSweepEnable,&WidgetButtons::clicked,this,&ArbGenPanelSettings::buttonSweepCallback);
    connect(dialFreqSweepMax,&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::sweepMaxCallback);
    connect(dialFreqSweepMin,&WidgetDialRange::valueChanged,this,&ArbGenPanelSettings::sweepMinCallback);

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
    isSilentGUIUpdate = true;
    for(int i = 0;i<buttonsEnable->getSelectedIndex()+1;i++){
        setChannelShown(i,true);
        channelEnabled[i] = true;
    }
    for(int i = buttonsEnable->getSelectedIndex()+1;i<MAX_ARB_CHANNELS_NUM;i++){
        setChannelShown(i,false);
        channelEnabled[i] = false;
    }

    memoryCallback(buttonsMemory->getSelectedIndex());
    customLengthInput->processInput();
    customLenghtCallback(customLengthInput->getValue());
    buttonSweepCallback(buttonSWSweepEnable->getSelectedIndex());

    for(int i =0;i<MAX_ARB_CHANNELS_NUM;i++){
        buttonShapeCallback(buttonsShape[i]->getSelectedIndex(),i);
        if(i>0 && swSyncWithCH1[i]->isCheckedRight()){
            syncWithCH1Callback(1,i);
        }
        if(i>0 && swSyncPWMWithCH1[i]->isCheckedRight()){
            syncPWMWithCH1Callback(1,i);
        }
        signalPWMFrequencyCallback(dialPWMFreqCh[i]->getRealValue(),i);
    }
    sweepMaxCallback(dialFreqSweepMax->getRealValue());
    isSilentGUIUpdate = false;
    signalChangedCallback();
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

void ArbGenPanelSettings::setPWMFreqLabel(QString freq, int index)
{
    labelRealPWMFreq[index]->setValue(freq);
}

void ArbGenPanelSettings::setPWMResolutionLabel(QString res, int index)
{
    labelPWMResolution[index]->setValue(res);
}

void ArbGenPanelSettings::disableGUI()
{
    GUIEnabled = false;
    for(int i = 0; i < MAX_ARB_CHANNELS_NUM; i++){
        //swSyncWithCH1[i]->setEnabled(false);
        buttonsShape[i]->setEnabled(false);
        //dialFreqCh[i]->setEnabled(false);
        dialOffsetCh[i]->setEnabled(false);
        dialAmplitudeCh[i]->setEnabled(false);
        dialDutyCh[i]->setEnabled(false);
        dialPhaseCh[i]->setEnabled(false);
        dialPWMFreqCh[i]->setEnabled(false);
    }
    buttonsEnable->setEnabled(false);
    buttonsMemory->setEnabled(false);
    customLengthInput->setEnabled(false);
    //buttonSWSweepEnable->setEnabled(false);
    //dialFreqSweepMax->setEnabled(false);
    //dialFreqSweepMin->setEnabled(false);
}

void ArbGenPanelSettings::enableGUI()
{
    GUIEnabled = true;
    for(int i = 0; i < MAX_ARB_CHANNELS_NUM; i++){
        //swSyncWithCH1[i]->setEnabled(true);
        buttonsShape[i]->setEnabled(true);
        //dialFreqCh[i]->setEnabled(true);
        dialOffsetCh[i]->setEnabled(true);
        dialAmplitudeCh[i]->setEnabled(true);
        dialDutyCh[i]->setEnabled(true);
        dialPhaseCh[i]->setEnabled(true);
        if(!channelSyncPWMWithCH1[i])
            dialPWMFreqCh[i]->setEnabled(true);
    }
    buttonsEnable->setEnabled(true);
    buttonsMemory->setEnabled(true);
    customLengthInput->setEnabled(true);
    //buttonSWSweepEnable->setEnabled(true);
    //dialFreqSweepMax->setEnabled(true);
    //dialFreqSweepMin->setEnabled(true);
    sweepMaxCallback(dialFreqSweepMax->getRealValue());
}

void ArbGenPanelSettings::copyFreq(int fromCh, int toCh)
{
    qreal value = dialFreqCh[fromCh]->getRealValue();
    dialFreqCh[toCh]->setRealValue(value,true);
}

void ArbGenPanelSettings::copyPWMFreq(int fromCh, int toCh)
{
    qreal value = dialPWMFreqCh[fromCh]->getRealValue();
    dialPWMFreqCh[toCh]->setRealValue(value,true);
}

void ArbGenPanelSettings::buttonEnableChannelCallback(int index)
{
    for(int i = 0;i<index+1;i++){
        setChannelShown(i,true);
        channelEnabled[i] = true;
    }
    for(int i = index+1;i<MAX_ARB_CHANNELS_NUM;i++){
        setChannelShown(i,false);
        channelEnabled[i] = false;
    }
    numChannelsEnabled = index+1;
    signalChangedCallback();
}

void ArbGenPanelSettings::buttonShapeCallback(int clicked, int channel)
{
    if(signalShape[channel] == SignalShape::ARB && clicked !=3){
        arbChannelsEnabled--;
    }

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
        arbChannelsEnabled++;
        dialDutyCh[channel]->hide();

        break;
    }

    if(arbChannelsEnabled>0){
        buttonSelectFile->show();
        labelArbFileInfo->show();
    }else{
        labelArbFileInfo->hide();
        buttonSelectFile->hide();
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::syncWithCH1Callback(int clicked, int channel)
{
    if(clicked==0){
        channelSyncWithCH1[channel] = false;
        dialFreqCh[channel]->setEnabled(true);
    }else{
        channelSyncWithCH1[channel] = true;
        copyFreq(0,channel);
        dialFreqCh[channel]->setEnabled(false);
    }
    signalChangedCallback();

    if(clicked != 0) emit syncRequest();
}

void ArbGenPanelSettings::syncPWMWithCH1Callback(int clicked, int channel)
{
    if(clicked==0){
        channelSyncPWMWithCH1[channel] = false;
        if(GUIEnabled)
            dialPWMFreqCh[channel]->setEnabled(true);
    }else{
        channelSyncPWMWithCH1[channel] = true;
        copyPWMFreq(0,channel);
        if (GUIEnabled)
            dialPWMFreqCh[channel]->setEnabled(false);
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::signalFrequencyCallback(qreal value, int channel)
{
    if(channel==0){
        for(int i = 1;i<MAX_ARB_CHANNELS_NUM;i++){
            if(channelSyncWithCH1[i])
                copyFreq(0,i);
        }
        dialFreqSweepMax->setRealValue(value,true);
    }else{
        if(channelSyncWithCH1[channel])
            dialFreqCh[0]->setRealValue(value,false);
    }

    if (channel == 0 || !channelSyncWithCH1[channel])
        signalChangedCallback();
}

void ArbGenPanelSettings::signalPWMFrequencyCallback(qreal value, int channel)
{
    setPWMFreqLabel(LabelFormator::formatOutout(value,"Hz",3),channel);
    if(channel==0){
        for(int i = 1;i<MAX_ARB_CHANNELS_NUM;i++){
            if(channelSyncPWMWithCH1[i])
                copyPWMFreq(0,i);
        }
    }else{
        if(channelSyncPWMWithCH1[channel])
            dialPWMFreqCh[0]->setRealValue(value,false);
    }

    if (channel == 0 || !channelSyncPWMWithCH1[channel])
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



void ArbGenPanelSettings::buttonSweepCallback(int index)
{
    if(index == 1){  //sweep is on
        dialFreqSweepMax->show();
        dialFreqSweepMin->show();
        dialFreqSweepTime->show();
        isSweepEnabled = true;
    }else{
        dialFreqSweepMax->hide();
        dialFreqSweepMin->hide();
        dialFreqSweepTime->hide();
        isSweepEnabled = false;
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::sweepMaxCallback(qreal value)
{
    if(value<dialFreqSweepMin->getRealValue())
        dialFreqSweepMin->setRealValue(value,true);
    sweepSettingsCallback();
}

void ArbGenPanelSettings::sweepMinCallback(qreal value)
{
    if(value>dialFreqSweepMax->getRealValue())
        dialFreqSweepMax->setRealValue(value,true);
    sweepSettingsCallback();
}

void ArbGenPanelSettings::sweepSettingsCallback()
{
    dialFreqCh[0]->setRealValue(dialFreqSweepMax->getRealValue(),true);
    for (int i = 1;i<MAX_ARB_CHANNELS_NUM ; i++) {
        if(swSyncWithCH1[i]->isCheckedRight()){
            dialFreqCh[i]->setRealValue(dialFreqSweepMax->getRealValue(),true);
        }
    }
    signalChangedCallback();
}

void ArbGenPanelSettings::signalChangedCallback()
{
    if(!isSilentGUIUpdate)
        emit signalChanged();
}