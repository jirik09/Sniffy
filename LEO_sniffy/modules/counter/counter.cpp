#include "counter.h"

Counter::Counter(QObject *parent/*, config param*/)
{
    Q_UNUSED(parent);
        //JIRI TODO parse config here (load config before making modules)
    config = new CounterConfig(this/*, param*/);
    cntWindow = new CounterWindow(config);

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

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
    connect(cntWindow->tabRatio->buttonRetrigger, &WidgetButtons::clicked, this, &Counter::ratRetriggerCallback);

    /* Intervals Counter Signals/Slots */
    connect(cntWindow->tabInter->buttonsStart, &WidgetButtons::clicked, this, &Counter::intButtonsStartCallback);
    connect(cntWindow->tabInter->dialTimeout, &WidgetDialRange::valueChanged, this, &Counter::intDialTimeoutChangedCallback);
    connect(cntWindow->tabInter->buttonsEventsSeq, &WidgetButtons::clicked, this, &Counter::intSwitchEventSequenceChangedCallback);
    connect(cntWindow->tabInter->switchEdgeEventA, &WidgetSwitch::clicked, this, &Counter::intEventAChangedCallback);
    connect(cntWindow->tabInter->switchEdgeEventB, &WidgetSwitch::clicked, this, &Counter::intEventBChangedCallback);
}

void Counter::startModule(){
    startCounting();
}

void Counter::stopModule(){
    stopCounting();
}

void Counter::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void Counter::holdCounter(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
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
        spec = new CounterSpec(dataToPass, this);
        cntWindow->setSpecification(spec);
        showModuleControl();

    }else {

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
    if(valToFormat != 0){
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
    if(errToFormat != 0)
        return display->formatNumber(errToFormat, 'g', 4);
    else
        return "0.0000";
}

void Counter::displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr){
    display->displayAvgString(avg);
    display->displayString(val);
    display->displayQerrString(qerr);
    display->displayTerrString(terr);
    QString color = (display == cntWindow->displayLFCh2) ? "grey" : "blue";
    display->drawIndicationFlag(LABELNUM_INDIC, color);
    cntWindow->displayFlagHoldOn(display, false);
}

void Counter::switchCounterModeCallback(int index){
    config->mode = (CounterMode)index;
    write(cmd->COUNTER_MODE, cmd->pCOUNTER_MODE.at(index));
    //reloadModeState[index];
    if(index == 0){
        hfReloadState();
    }else if (index == 1) {
        lfReloadState();
    }else if (index == 2) {
        ratReloadState();
    }else if (index == 3) {
        intReloadState();
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
    QString strVal, strAvg, strQerr, strTerr;
    QDataStream streamBuffLeng(data);
    streamBuffLeng >> val >> qerr >> terr;

    WidgetDisplay *display = cntWindow->displayHF;

    bool isFrequency = (freqPer == "QFRE") ? true : false;

    uint countToGo = (isFrequency) ? movAvg->prepend(val) : movAvg->prepend(1/val);
    this->strQerr = strQerr = formatErrNumber(display, qerr);
    this->strTerr = strTerr = formatErrNumber(display, terr);
    cntWindow->showPMErrorSigns(display, true);

    bool isAvgBuffFull = movAvg->isBufferFull();

    if(isAvgBuffFull){
        config->hfState.hold = HFState::HoldOnState::OFF;
        cntWindow->hfSetColorRemainSec(QCOLOR_WHITE);
        avg = (isFrequency) ? movAvg->getAverage() : 1 / movAvg->getAverage();
        if (config->hfState.error == HFState::ErrorType::AVERAGE){
            qerr = (isFrequency) ? qerr / movAvg->getBufferSize() : 1 / (1 / avg - movAvg->getBufferSize()) - avg;
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

    if(!isFrequency)
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
    cntWindow->hfSetColorRemainSec(QCOLOR_GREY);
    config->hfState.hold = HFState::HoldOnState::ON;
    return (formatTime + " Sec ");
}

void Counter::hfDisplayErrors(){
    if(config->hfState.error == HFState::ErrorType::SIMPLE){
        cntWindow->displayFlagHoldOn(cntWindow->displayHF, true);
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
    write(cmd->HF_QUANTITY, cmd->pQUANTITIY.at(index));
    cntWindow->clearDisplay(cntWindow->displayHF, true);
    cntWindow->displayHF->displayAvgString("");
}

void Counter::hfSwitchGateTimeCallback(int index){
    config->hfState.gateTimeIndexBackup = index;
    write(cmd->HF_GATE_TIME, cmd->pHF_GATE_TIME.at(index));

    if(index == 0){
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    }else if(index == 1){
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_500M;
    }else if(index == 2){
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_1S;
    }else if(index == 3){
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_5S;
    }else if(index == 4){
        config->hfState.gateTime = HFState::GateTime::GATE_TIME_10S;
    }
    movAvg->clear();
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayHF->displayAvgString(seconds);
    cntWindow->clearDisplay(cntWindow->displayHF, true);
}

void Counter::hfSwitchErrorAvgCallback(int index){
    config->hfState.error = (HFState::ErrorType)index;
    hfDisplayErrors();
}

void Counter::hfDialAvgChangedCallback(float val){
    movAvg->setBufferSize(val);
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayHF->displayAvgString(seconds);
    hfDisplayErrors();
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

    WidgetDisplay *display = (channel == "IC1D") ? cntWindow->displayLFCh1 : cntWindow->displayLFCh2;
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
            display->updateProgressBar(spec->lf_max);
        }else {
            displayValues(display, strVal, "", strQerr, strTerr);
            cntWindow->showPMErrorSigns(display, true);
            cntWindow->displayFlagHoldOn(display, false);
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
        cntWindow->displayFlagHoldOn(display, false);
        displayValues(display, strVal, strVal2, "", "");

        /* History section */
        cntWindow->appendNewHistorySample(display, "PW ", val1, " s");
        cntWindow->associateToHistorySample(display, 1, ", DC ", val2, " \%");
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
    return (frequency > spec->lf_max) ? true : false;
}

void Counter::lfSwitchChannelCallback(int index){
    config->lfState.activeChan = LFState::ActiveChan(index);
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
    write(channelQuantitiy, cmd->pQUANTITIY.at(index));
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
    write(channelMultiplier, cmd->pLF_MULTIPLIER.at(index));
    if(config->lfState.activeChan == LFState::ActiveChan::CHAN1){
        cntWindow->clearDisplay(cntWindow->displayLFCh1, true);
    }else {
        cntWindow->clearDisplay(cntWindow->displayLFCh2, true);
    }
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
}

void Counter::lfDialSampleCountCh1ChangedCallback(float val){
    config->lfState.chan1.sampleCount = val;
    write(cmd->LF_CH1_SAMPLE_COUNT, val);
}

void Counter::lfDialSampleCountCh2ChangedCallback(float val){
    config->lfState.chan2.sampleCount = val;
    write(cmd->LF_CH2_SAMPLE_COUNT, val);
}

/************************************** RATIO MEAS. FUNCTIONS ****************************************/
void Counter::parseRatioCounter(QByteArray data){
    QByteArray inputString = data.left(4); data.remove(0, 4);
    WidgetDisplay *display = cntWindow->displayRat;

    if(inputString == "WARN"){
        cntWindow->ratDisplayFlagWarning(display, true);
    }else {
        double val; QString strVal;
        QDataStream streamBuffLeng(data);
        streamBuffLeng >> val;

        strVal = display->formatNumber(val, 'g', 5);
        display->displayString(strVal);
        double error = 1 / (double)config->ratState.sampleCount;
        strVal = display->formatNumber(error, 'g', 6);
        display->displayTerrString(strVal);

        cntWindow->displayFlagHoldOn(display, false);
        cntWindow->showPMErrorSigns(display, true);
        display->drawIndicationFlag(LABELNUM_INDIC, "blue");

        /* History section */
        cntWindow->appendNewHistorySample(display, "Ratio: ", val, "");
    }
}

void Counter::ratReloadState(){
    ratDialSampleCountChangedCallback(config->ratState.sampleCount);
}

void Counter::ratDialSampleCountChangedCallback(float val){
    cntWindow->clearDisplay(cntWindow->displayRat, true);
    config->ratState.sampleCount = val;
    write(cmd->RAT_CH3_SAMPLE_COUNT, val);
}

void Counter::ratRetriggerCallback(int index){
    Q_UNUSED(index);
    cntWindow->clearDisplay(cntWindow->displayRat, true);
    write(cmd->RAT_CH3_SAMPLE_COUNT, config->ratState.sampleCount);
}

/************************************** INTERVALS FUNCTIONS ****************************************/
void Counter::parseIntervalsCounter(QByteArray data){
    QByteArray inputString = data.left(4); data.remove(0, 4);
    WidgetDisplay *display = cntWindow->displayInt;

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
        cntWindow->displayFlagHoldOn(display, false);

        /* History section */
        QString pm(0x00B1);
        cntWindow->appendNewHistorySample(display, "Time interval: ", val, " Sec");
        cntWindow->associateToHistorySample(display, 1, ", " + pm + "qerr ", qerr);
        cntWindow->associateToHistorySample(display, 2, " " + pm + "terr ", terr);
    }
}

void Counter::intReloadState(){
    intSwitchEventSequenceChangedCallback((int)config->intState.seqAB);
    intDialTimeoutChangedCallback(config->intState.timeout);
    intEventAChangedCallback((int)config->intState.eventA);
    intEventBChangedCallback((int)config->intState.eventB);
}

void Counter::intButtonsStartCallback(){
    comm->write(moduleCommandPrefix+":"+cmd->START+";");
    cntWindow->displayFlagHoldOn(cntWindow->displayInt, true);
    cntWindow->clearDisplay(cntWindow->displayInt, true);
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

