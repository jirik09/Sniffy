#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);
    config = new CounterConfig(this);
    cntWindow = new CounterWindow(config);

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_counter.png";

    movAvg = new MovingAverage(2, cntWindow->tabHighFreq);       

    /* Common Counter Signals/Slots */
    connect(this, &AbstractModule::moduleCreated, this, &Counter::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &Counter::holdCounter);
    connect(cntWindow->tabs, &widgetTab::tabBarClicked, this, &Counter::switchCounterModeCallback);

    /* High Frequency Counter Signals/Slots */
    connect(cntWindow->tabHighFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &Counter::hfSwitchQuantityCallback);
    connect(cntWindow->tabHighFreq->buttonsGateTime, &WidgetButtons::clicked, this, &Counter::hfSwitchGateTimeCallback);
    connect(cntWindow->tabHighFreq->buttonsErrorSwitch, &WidgetButtons::clicked, this, &Counter::hfSwitchErrorAvgCallback);
    connect(cntWindow->tabHighFreq->dialAveraging, &WidgetDialRange::valueChanged, this, &Counter::hfDialAvgChangedCallback);

    /* Low Frequency Counter Signals/Slots */
    connect(cntWindow->tabLowFreq->buttonsChannelSwitch, &WidgetButtons::clicked, this, &Counter::lfSwitchChannelCallback);
    connect(cntWindow->tabLowFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &Counter::lfSwitchQuantityCallback);
    connect(cntWindow->tabLowFreq->buttonsMultiplierSwitch, &WidgetButtons::clicked, this, &Counter::lfSwitchMultiplierCallback);
    connect(cntWindow->tabLowFreq->buttonsDutyCycleSwitch, &WidgetButtons::clicked, this, &Counter::lfSwitchDutyCycleCallback);
    connect(cntWindow->tabLowFreq->dialSampleCountCh1, &WidgetDialRange::valueChanged, this, &Counter::lfDialSampleCountCh1ChangedCallback);
    connect(cntWindow->tabLowFreq->dialSampleCountCh2, &WidgetDialRange::valueChanged, this, &Counter::lfDialSampleCountCh2ChangedCallback);

    /* Ratio Counter Signals/Slots */
    connect(cntWindow->tabRatio->dialSampleCount, &WidgetDialRange::valueChanged, this, &Counter::ratDialSampleCountChangedCallback);    

    /* Intervals Counter Signals/Slots */
    connect(cntWindow->tabInter->buttonStart, &WidgetButtons::clicked, this, &Counter::intButtonsStartCallback);
    connect(cntWindow->tabInter->dialTimeout, &WidgetDialRange::valueChanged, this, &Counter::intDialTimeoutChangedCallback);
    connect(cntWindow->tabInter->buttonsEventsSeq, &WidgetButtons::clicked, this, &Counter::intSwitchEventSequenceChangedCallback);
    connect(cntWindow->tabInter->switchEdgeEventA, &WidgetSwitch::clicked, this, &Counter::intEventAChangedCallback);
    connect(cntWindow->tabInter->switchEdgeEventB, &WidgetSwitch::clicked, this, &Counter::intEventBChangedCallback);
}

void Counter::startModule(){}

void Counter::stopModule(){
    stopCounting();
}

void Counter::showHoldButtonCallback(){
    this->showModuleHoldButton(true);
}

void Counter::holdCounter(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
        config->isHeld = true;
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
        config->isHeld = false;
    }
}

void Counter::startCounting(){
    comm->write(moduleCommandPrefix+":"+cmd->START+";");
    setModuleStatus(ModuleStatus::PLAY);
}

void Counter::stopCounting(){
    comm->write(moduleCommandPrefix+":"+cmd->STOP+";");
    setModuleStatus(ModuleStatus::PAUSE);
}

void Counter::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.remove(0, 4);

    if(dataHeader == "CFG_"){
        moduleSpecification = new CounterSpec(this);
        moduleSpecification->parseSpecification(dataToPass);
        cntWindow->setSpecification(static_cast<CounterSpec*>(moduleSpecification));
        showModuleControl();

    }else {
        if(config->isHeld)
            return;

        if(dataHeader == "HF_D"){
            parseHighFrequencyCounter(dataToPass);
        }else if(dataHeader=="LF_D"){
            parseLowFrequencyCounter(dataToPass);
        }else if(dataHeader=="RF_D"){
            parseRatioCounter(dataToPass);
        }else if(dataHeader=="TI_D"){
            parseIntervalsCounter(dataToPass);
        }
    }
}

/************************************** COMMON FUNCTIONS ****************************************/
QString Counter::formatNumber(WidgetDisplay *display, double valToFormat, double error){
    QString str = "0.000";
    if(valToFormat > 0){
        const int places = 4;
        int abs = qFabs(error);
        int leftDigitNum = (abs < 1) ? 0 : (int)(log10(abs) + 1);

        if(valToFormat < 1){
            str = display->formatNumber(valToFormat, 'e', 6);
        }else if(leftDigitNum >= places){
            str = display->formatNumber(valToFormat, 'f', 0);
        }else if (leftDigitNum < places) {
            int rightDigitNum = places - leftDigitNum;
            str = display->formatNumber(valToFormat, 'f', rightDigitNum);
        }
    }
    return str;
}

QString Counter::formatErrNumber(WidgetDisplay *display, double errToFormat){
    if(errToFormat > 0)
        return display->formatNumber(errToFormat, 'g', 4);
    else
        return "0.0000";
}

void Counter::displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr){
    display->displayAvgString(avg);
    display->displayString(val);
    display->displayQerrString(qerr);
    display->displayTerrString(terr);

    QString color;
    if(display == cntWindow->displayLFCh2)
        color = Graphics::getChannelColor(1);
    else
        color = Graphics::getChannelColor(0);

    display->drawIndicationFlag(LABELNUM_INDIC);
    cntWindow->displayFlagAcquiring(display, false);
}

void Counter::switchCounterModeCallback(int index){
    config->mode = (CounterMode)index;

    if(index == 0){
        write(cmd->COUNTER_MODE, cmd->MODE_HIGH_FREQ);                        
        hfReloadState();
    }else if (index == 1) {
        write(cmd->COUNTER_MODE, cmd->MODE_LOW_FREQ);                
        lfReloadState();
    }else if (index == 2) {
        write(cmd->COUNTER_MODE, cmd->MODE_RATIO);                
        ratReloadState();
    }else if (index == 3) {
        write(cmd->COUNTER_MODE, cmd->MODE_INTERVAL);                
        intReloadState();
    }    

    if(config->mode != CounterMode::INTERVAL){
        startCounting();
        this->showModuleHoldButton(true);
    }else {
        setModuleStatus(ModuleStatus::PAUSE);
        this->showModuleHoldButton(false);
    }

    discardHold();
}

void Counter::discardHold(){
    if(config->isHeld){
        enableModuleHoldButton(false);
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        config->isHeld = false;
    }
}

void Counter::write(QByteArray feature, QByteArray param){
    comm->write(moduleCommandPrefix, feature, param);
}

void Counter::write(QByteArray feature, int param){
    comm->write(moduleCommandPrefix, feature, param);
}

/************************************** HIGH FREQ FUNCTIONS ****************************************/
void Counter::parseHighFrequencyCounter(QByteArray data){
    QByteArray freqPer = data.left(4); data.remove(0, 4);

    double val, qerr, terr;
    QString strAvg, strQerr, strTerr; // strVal
    QDataStream streamBuffLeng(data);
    streamBuffLeng >> val >> qerr >> terr;

    WidgetDisplay *display = cntWindow->displayHF;

    bool isFrequency = (freqPer == "QFRE") ? true : false;

    uint countToGo = (isFrequency || val==0) ? movAvg->prepend(val) : movAvg->prepend(1/val);
    this->strQerr = strQerr = formatErrNumber(display, qerr);
    this->strTerr = strTerr = formatErrNumber(display, terr);
    cntWindow->showPMErrorSigns(display, true);

    bool isAvgBuffFull = movAvg->isBufferFull();

    if(isAvgBuffFull){
        config->hfState.hold = HFState::HoldOnState::OFF;
        cntWindow->hfSetColorRemainSec(false);
        avg = (isFrequency || movAvg->getAverage()==0) ? movAvg->getAverage() : 1 / movAvg->getAverage();
        if (config->hfState.error == HFState::ErrorType::AVERAGE){
            qerr = (isFrequency || avg<=0) ? qerr / movAvg->getBufferSize() : 1 / (1 / avg - movAvg->getBufferSize()) - avg;
            avgQerr = strQerr = formatErrNumber(display, qerr);
        }
        strAvg = formatNumber(display, avg, qerr+terr);
    }else {
        if(config->hfState.error == HFState::ErrorType::AVERAGE){
            cntWindow->showPMErrorSigns(display, false);
            strQerr = " ";
            strTerr = " ";
        }
        strAvg = hfFormatRemainSec(countToGo, 0);
    }

    displayValues(display, formatNumber(display, val, qerr+terr), strAvg, strQerr, strTerr);

    /* History section */
    QString pm(0x00B1);
    QString quant = (isFrequency) ? " Hz": " s";

    cntWindow->appendNewHistorySample(display, "", val, quant, (float)config->hfState.gateTime/(float)1000);
    cntWindow->associateToHistorySample(display, 1, ", " + pm + "qerr ", qerr, quant);
    cntWindow->associateToHistorySample(display, 2, " " + pm + "terr ", terr, quant);

    if(isAvgBuffFull)
        cntWindow->associateToHistorySample(display, 3, ", avg = ", avg, quant);
    else
        cntWindow->associateToHistorySample(display, 3, ", avg unavailable ", -1);

    if(!isFrequency && val>0)
        val = 1 / val;

    display->updateProgressBar(val);
    config->hfState.quantState = HFState::QuantitySwitched::NO;
}

void Counter::hfReloadState(){    
    hfSwitchQuantityCallback((int)config->hfState.quantity);
    hfSwitchGateTimeCallback(config->hfState.gateTimeIndexBackup);
}

QString Counter::hfFormatRemainSec(uint countToGo, uint additionTime){
    double timeInSec = (countToGo * (int)config->hfState.gateTime) / (double)1000 + additionTime;
    QString formatTime;
    if(config->hfState.gateTime < HFState::GateTime::GATE_TIME_1S)
        formatTime = QString::number(timeInSec, 'f', 1);
    else
        formatTime = QString::number(timeInSec, 'd', 0);
    cntWindow->hfSetColorRemainSec(true);
    config->hfState.hold = HFState::HoldOnState::ON;
    return (formatTime + " Sec ");
}

void Counter::hfDisplayErrors(){
    if(config->hfState.error == HFState::ErrorType::SIMPLE){
        cntWindow->displayFlagAcquiring(cntWindow->displayHF, true);
        cntWindow->displayHF->displayQerrString(" ");
        cntWindow->displayHF->displayTerrString(" ");
        cntWindow->showPMErrorSigns(cntWindow->displayHF, false);
    }else if (config->hfState.error == HFState::ErrorType::AVERAGE) {
        if(config->hfState.hold == HFState::HoldOnState::OFF){
            cntWindow->displayHF->displayQerrString(avgQerr);
            cntWindow->displayHF->displayTerrString(strTerr);
            cntWindow->showPMErrorSigns(cntWindow->displayHF, true);
        }else{
            cntWindow->displayHF->displayQerrString(" ");
            cntWindow->displayHF->displayTerrString(" ");
            cntWindow->showPMErrorSigns(cntWindow->displayHF, false);
        }
    }
}

void Counter::hfSwitchQuantityCallback(int index){
    config->hfState.quantState = HFState::QuantitySwitched::YES;
    config->hfState.quantity = (HFState::Quantity)index;

    if(config->hfState.quantity == HFState::Quantity::FREQUENCY)
        write(cmd->HF_QUANTITY, cmd->QUANT_FREQUENCY);
    else
        write(cmd->HF_QUANTITY, cmd->QUANT_PERIOD);

    cntWindow->clearDisplay(cntWindow->displayHF, true);
    cntWindow->displayHF->displayAvgString("");

    discardHold();
}

void Counter::hfSwitchGateTimeCallback(int index){
    config->hfState.gateTimeIndexBackup = index;

    if(index == 0){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_100M);
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    }else if(index == 1){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_500M);
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_500M;
    }else if(index == 2){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_1S);
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_1S;
    }else if(index == 3){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_5S);
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_5S;
    }else if(index == 4){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_10S);
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_10S;
    }
    movAvg->clear();
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayHF->displayAvgString(seconds);
    cntWindow->clearDisplay(cntWindow->displayHF, true);

    discardHold();
}

void Counter::hfSwitchErrorAvgCallback(int index){
    config->hfState.error = (HFState::ErrorType)index;
    hfDisplayErrors();

    discardHold();
}

void Counter::hfDialAvgChangedCallback(float val){
    movAvg->setBufferSize(val);
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayHF->displayAvgString(seconds);
    hfDisplayErrors();

    discardHold();
}

/************************************** LOW FREQ FUNCTIONS ****************************************/
void Counter::parseLowFrequencyCounter(QByteArray data){
    QByteArray mode = data.left(4);
    QByteArray channel = data.mid(4, 4);
    QByteArray quantity = data.mid(8, 4);
    data.remove(0, 12);

    double val1, val2, qerr, terr;
    QString strVal, strVal2, strQerr, strTerr;
    QDataStream streamBuffLeng(data);
    streamBuffLeng >> val1 >> val2 >> qerr >> terr;

    WidgetDisplay *display = cntWindow->displayLFCh1;
    if(channel == "IC1D"){
        display = cntWindow->displayLFCh1;
    }else if(channel == "IC2D"){
        display = cntWindow->displayLFCh2;
    }else {
        mode = "xx";
    }

    QString pm(0x00B1);

    if(mode == "FPME"){
        strVal = formatNumber(display, val1, qerr+terr);
        strQerr = formatErrNumber(display, qerr);
        strTerr = formatErrNumber(display, terr);

        bool isFrequency = (quantity != "QPER");
        if(!isFrequency)
            val1 = 1 / val1;

        if(isRangeExceeded(val1)){
            cntWindow->clearDisplay(display, false);
            cntWindow->displayFlagSwitchMode(display, true);
            display->updateProgressBar(static_cast<CounterSpec*>(moduleSpecification)->lf_max);
        }else {
            displayValues(display, strVal, "", strQerr, strTerr);
            cntWindow->showPMErrorSigns(display, true);
            cntWindow->displayFlagAcquiring(display, false);
            display->updateProgressBar(val1);

            QString quant;
            if(isFrequency){
                quant = " Hz";
            }else {
                quant = " s";
                val1 = 1 / val1;
            }

            /* History section */
            cntWindow->appendNewHistorySample(display, "", val1, quant);
            cntWindow->associateToHistorySample(display, 1, ", " + pm + "qerr ", qerr);
            cntWindow->associateToHistorySample(display, 2, " " + pm + "terr ", terr);
        }
    }else if (mode == "DUTY") {
        strVal = display->formatNumber(val1, 'f', 3);
        strVal2 = display->formatNumber(val2, 'e', 5);
        cntWindow->showPMErrorSigns(display, true);
        cntWindow->displayFlagAcquiring(display, false);
        displayValues(display, strVal, strVal2, "", "");

        /* History section */
        cntWindow->appendNewHistorySample(display, "PW ", val1, " s");
        cntWindow->associateToHistorySample(display, 1, ", DC ", val2, " \%");
        cntWindow->associateToHistorySample(display, 2, " ");
    }
}

void Counter::lfReloadState(){    
    LFState::Channel chan = (config->lfState.activeChan == LFState::ActiveChan::CHAN1) ? config->lfState.chan1 : config->lfState.chan2;
    if (chan.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
        lfSwitchDutyCycleCallback((int)LFState::Channel::DutyCycle::ENABLED);
    }else {
        lfReloadStateQuantMeasurement();
    }
}

void Counter::lfReloadStateQuantMeasurement(){
    lfSwitchQuantity((int)config->lfState.chan1.quantity, cmd->LF_CH1_QUANTITY);
    lfSwitchQuantity((int)config->lfState.chan2.quantity, cmd->LF_CH2_QUANTITY);
    lfSwitchMultiplier((int)config->lfState.chan1.multiplier, cmd->LF_CH1_MULTIPLIER);
    lfSwitchMultiplier((int)config->lfState.chan2.multiplier, cmd->LF_CH2_MULTIPLIER);
    lfDialSampleCountCh1ChangedCallback(config->lfState.chan1.sampleCount);
    lfDialSampleCountCh2ChangedCallback(config->lfState.chan2.sampleCount);
}

bool Counter::isRangeExceeded(double frequency){
    return (frequency > static_cast<CounterSpec*>(moduleSpecification)->lf_max) ? true : false;
}

void Counter::lfSwitchChannelCallback(int index){
    config->lfState.activeChan = LFState::ActiveChan(index);
    discardHold();
}

void Counter::lfSwitchQuantityCallback(int index){
    QByteArray chanQuant;
    WidgetDisplay *display = cntWindow->displayLFCh1;

    if(config->lfState.activeChan == LFState::ActiveChan::CHAN1){
        config->lfState.chan1.quantity = (LFState::Channel::Quantity)index;
        display = cntWindow->displayLFCh1;
        chanQuant = cmd->LF_CH1_QUANTITY;
    }else if (config->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        config->lfState.chan2.quantity = (LFState::Channel::Quantity)index;
        display = cntWindow->displayLFCh2;
        chanQuant = cmd->LF_CH2_QUANTITY;
    }

    lfSwitchQuantity(index, chanQuant);
    cntWindow->clearDisplay(display, true);        
}

void Counter::lfSwitchQuantity(int index, QByteArray channelQuantitiy){
    if(index == 0)
        write(channelQuantitiy, cmd->QUANT_FREQUENCY);
    else
        write(channelQuantitiy, cmd->QUANT_PERIOD);

    discardHold();
}

void Counter::lfSwitchMultiplierCallback(int index){
    QByteArray multip;
    if(config->lfState.activeChan == LFState::ActiveChan::CHAN1){
        config->lfState.chan1.multiplier = (LFState::Channel::Multiplier)index;
        multip = cmd->LF_CH1_MULTIPLIER;
    }else if (config->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        config->lfState.chan2.multiplier = (LFState::Channel::Multiplier)index;
        multip = cmd->LF_CH2_MULTIPLIER;
    }

    lfSwitchMultiplier(index, multip);        
}

void Counter::lfSwitchMultiplier(int index, QByteArray channelMultiplier){
    switch(index){
    case 0:
        write(channelMultiplier, cmd->LF_MULTIPLIER_1X);
        break;
    case 1:
        write(channelMultiplier, cmd->LF_MULTIPLIER_2X);
        break;
    case 2:
        write(channelMultiplier, cmd->LF_MULTIPLIER_4X);
        break;
    case 3:
        write(channelMultiplier, cmd->LF_MULTIPLIER_8X);
        break;
    default:
        break;
    }    

    if(config->lfState.activeChan == LFState::ActiveChan::CHAN1){
        cntWindow->clearDisplay(cntWindow->displayLFCh1, true);
    }else {
        cntWindow->clearDisplay(cntWindow->displayLFCh2, true);
    }

    discardHold();
}

void Counter::lfSwitchDutyCycleCallback(int index){
    QByteArray chanDutyCycle;
    if(config->lfState.activeChan == LFState::ActiveChan::CHAN1){
        config->lfState.chan1.dutyCycle = (LFState::Channel::DutyCycle)index;
        chanDutyCycle = cmd->LF_CH1_DUTY_CYCLE_ENABLE;
    }else if (config->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        config->lfState.chan2.dutyCycle = (LFState::Channel::DutyCycle)index;
        chanDutyCycle = cmd->LF_CH2_DUTY_CYCLE_ENABLE;
    }

    if(index == (int)LFState::Channel::DutyCycle::DISABLED){
        write(cmd->LF_DUTY_CYCLE, cmd->LF_DUTY_CYCLE_DISABLE);
        lfReloadStateQuantMeasurement();
    }else if (index == (int)LFState::Channel::DutyCycle::ENABLED) {
        write(cmd->LF_DUTY_CYCLE, chanDutyCycle);
    }

    discardHold();
}

void Counter::lfDialSampleCountCh1ChangedCallback(float val){
    config->lfState.chan1.sampleCount = val;
    write(cmd->LF_CH1_SAMPLE_COUNT, val);
    discardHold();
}

void Counter::lfDialSampleCountCh2ChangedCallback(float val){
    config->lfState.chan2.sampleCount = val;
    write(cmd->LF_CH2_SAMPLE_COUNT, val);
    discardHold();
}

/************************************** RATIO MEAS. FUNCTIONS ****************************************/
void Counter::parseRatioCounter(QByteArray data){    
    QByteArray inputString = data.left(4); data.remove(0, 4);
    WidgetDisplay *display = cntWindow->displayRat;    

    if(inputString == "WARN"){
        cntWindow->clearDisplay(display, false);
        cntWindow->ratDisplayFlagWarning(display, true);        
        setModuleStatus(ModuleStatus::PAUSE);        
    }else {
        double val; QString strVal;
        QDataStream streamBuffLeng(data);
        streamBuffLeng >> val;

        strVal = display->formatNumber(val, 'g', 5);
        display->displayString(strVal);
        double error = 1 / (double)config->ratState.sampleCount;
        strVal = display->formatNumber(error, 'g', 6);
        display->displayTerrString(strVal);

        cntWindow->displayFlagAcquiring(display, false);
        cntWindow->showPMErrorSigns(display, true);
        display->drawIndicationFlag(LABELNUM_INDIC);

        /* History section */
        cntWindow->appendNewHistorySample(display, "Ratio: ", val, "");
    }
}

void Counter::ratReloadState(){    
    ratDialSampleCountChangedCallback(config->ratState.sampleCount);
}

void Counter::ratDialSampleCountChangedCallback(float val){    
    config->ratState.sampleCount = val;
    write(cmd->RAT_CH3_SAMPLE_COUNT, val);
    startCounting();
    discardHold();
    cntWindow->clearDisplay(cntWindow->displayRat, true);
}

/************************************** INTERVALS FUNCTIONS ****************************************/
void Counter::parseIntervalsCounter(QByteArray data){
    QByteArray inputString = data.left(4); data.remove(0, 4);
    WidgetDisplay *display = cntWindow->displayInt;

    cntWindow->tabInter->setStartButton(false);
    cntWindow->tabInter->enableConfigButtons(true);
    config->intState.running = false;
    setModuleStatus(ModuleStatus::PAUSE);

    if(inputString == "TMOT"){
        cntWindow->intDisplayFlagTimeout(display, true);
    }else {
        double val, qerr, terr;
        QString strVal, strQerr, strTerr;
        QDataStream streamBuffLeng(data);
        streamBuffLeng >> val >> qerr >> terr;

        strVal = formatNumber(display, val, qerr+terr);
        strQerr = formatErrNumber(display, qerr);
        strTerr = formatErrNumber(display, terr);

        displayValues(display, strVal, "", strQerr, strTerr);

        cntWindow->showPMErrorSigns(display, true);
        cntWindow->displayFlagAcquiring(display, false);

        /* History section */
        QString pm(0x00B1);
        cntWindow->appendNewHistorySample(display, "Time interval: ", val, " Sec");
        cntWindow->associateToHistorySample(display, 1, ", " + pm + "qerr ", qerr);
        cntWindow->associateToHistorySample(display, 2, " " + pm + "terr ", terr);
    }
}

void Counter::intReloadState(){
    config->intState.running = false;
    intSwitchEventSequenceChangedCallback((int)config->intState.seqAB);
    intDialTimeoutChangedCallback(config->intState.timeout);
    intEventAChangedCallback((int)config->intState.eventA);
    intEventBChangedCallback((int)config->intState.eventB);
}

void Counter::intButtonsStartCallback(){ 
    if(config->intState.running){
        stopCounting();
        cntWindow->tabInter->setStartButton(!config->intState.running);
        cntWindow->tabInter->enableConfigButtons(true);
        cntWindow->displayFlagAcquiring(cntWindow->displayInt, false);
        config->intState.running = false;
    }else {
        startCounting();
        cntWindow->tabInter->setStartButton(!config->intState.running);
        cntWindow->tabInter->enableConfigButtons(false);
        cntWindow->clearDisplay(cntWindow->displayInt, true);        
        config->intState.running = true;
    }
}

void Counter::intSwitchEventSequenceChangedCallback(int index){    
    if(index){
        write(cmd->INT_EVENT, cmd->INT_EVENT_SEQUENCE_BA);
    }else {
        write(cmd->INT_EVENT, cmd->INT_EVENT_SEQUENCE_AB);
    }
    config->intState.seqAB = (IntState::Sequence)index;
}

void Counter::intEventAChangedCallback(int index){
    if(index){
        write(cmd->INT_EVENT, cmd->INT_EVENT_FALL_CH1);
    }else {
        write(cmd->INT_EVENT, cmd->INT_EVENT_RISE_CH1);
    }
    config->intState.eventA = (IntState::Event)index;
}

void Counter::intEventBChangedCallback(int index){
    if(index){
        write(cmd->INT_EVENT, cmd->INT_EVENT_FALL_CH2);
    }else {
        write(cmd->INT_EVENT, cmd->INT_EVENT_RISE_CH2);
    }
    config->intState.eventB = (IntState::Event)index;
}

void Counter::intDialTimeoutChangedCallback(float val){
    config->intState.timeout = val;
    write(cmd->INT_TIMEOUT_SEC, val);
}

void Counter::writeConfiguration(){ //tahle funkce se vola vzdy pri otevreni modulu
    cntWindow->restoreGUIAfterStartup();
    switchCounterModeCallback((int)config->mode);
}

void Counter::parseConfiguration(QByteArray config){    
    this->config->parse(config);
}

QByteArray Counter::getConfiguration(){
    return config->serialize();
}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}

