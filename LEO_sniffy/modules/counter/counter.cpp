#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);

    loc = QLocale(QLocale::English);

    conf = new CounterConfig(this);
    cntWindow = new CounterWindow(conf);

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

    movAvg = new MovingAverage(2, cntWindow->tabHighFreq);

    /* Common Counter Signals/Slots */
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
}

void Counter::startModule(){
    startCounting();
}

void Counter::stopModule(){
    stopCounting();
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
        showModuleControl();
        spec = new CounterSpec(dataToPass, this);
        cntWindow->setSpecification(spec);
    }else {

        if(dataHeader == "HF_D"){
            parseHighFrequencyCounter(dataToPass);
        }else if(dataHeader=="LF_D"){
            parseLowFrequencyCounter(dataToPass);
        }else if(dataHeader=="RF_D"){
            parseRatioCounter(dataToPass);
        }else if(dataHeader=="TI_D"){

        }
    }
}

/************************************** COMMON FUNCTIONS ****************************************/
QString Counter::formatNumber(double valToFormat, double error){
    QString str;
    const int places = 5;
    int abs = qFabs(error);
    int leftDigitNum = (abs < 1) ? 0 : (int)(log10(abs) + 1);

    if(valToFormat < 1){
        str = loc.toString(valToFormat, 'e');
    }else if(leftDigitNum >= places){
        str = loc.toString(valToFormat, 'f', 0);
    }else if (leftDigitNum < places) {
        int rightDigitNum = places - leftDigitNum;
        str = loc.toString(valToFormat, 'f', rightDigitNum);
    }
    str.replace(loc.decimalPoint(), '.');

    return str;
}

void Counter::displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr){
    display->displayAvgString(avg);
    display->displayString(val);
    display->displayQerrString(qerr);
    display->displayTerrString(terr);
    QString color = (display == cntWindow->displayLFCh2) ? "grey" : "blue";
    display->drawIndicationFlag(LABELNUM_FLAG, color);
    cntWindow->displayFlagHoldOn(display, false);
}

void Counter::switchCounterModeCallback(int index){
    conf->mode = (CounterMode)index;
    write(cmd->COUNTER_MODE, cmd->pCOUNTER_MODE.at(index));
    //reloadModeState[index];
    if(index == 0){
        hfReloadState();
    }else if (index == 1) {
        lfReloadState();
    }else if (index == 2) {
        ratReloadState();
    }else if (index == 3) {

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

    bool isFrequency = (freqPer == "QFRE") ? true : false;
    uint countToGo = (isFrequency) ? movAvg->prepend(val) : movAvg->prepend(1/val);
    this->strQerr = strQerr = loc.toString(qerr, 'g', 3).replace(loc.decimalPoint(), '.');
    this->strTerr = strTerr = loc.toString(terr, 'g', 3).replace(loc.decimalPoint(), '.');
    cntWindow->showPMErrorSigns(cntWindow->displayHF, true);

    if(movAvg->isBufferFull()){
        conf->hfState.hold = HFState::HoldOnState::OFF;
        cntWindow->hfSetColorRemainSec(QCOLOR_WHITE);
        double avg = (isFrequency) ? movAvg->getAverage() : 1 / movAvg->getAverage();
        if (conf->hfState.error == HFState::ErrorType::AVERAGE){
            qerr = (isFrequency) ? qerr / movAvg->getBufferSize() : 1 / (1 / avg - movAvg->getBufferSize()) - avg;
            avgQerr = strQerr = loc.toString(qerr, 'g', 3).replace(loc.decimalPoint(), '.');
        }
        strAvg = formatNumber(avg, qerr+terr);
    }else {
        if(conf->hfState.error == HFState::ErrorType::AVERAGE){
            cntWindow->showPMErrorSigns(cntWindow->displayHF, false);
            strQerr = " ";
            strTerr = " ";
        }
        strAvg = hfFormatRemainSec(countToGo, 0);
    }

    displayValues(cntWindow->displayHF, formatNumber(val, qerr+terr), strAvg, strQerr, strTerr);
    val = (isFrequency) ? val : 1 / val;
    cntWindow->displayHF->updateProgressBar(val);
    conf->hfState.quantState = HFState::QuantitySwitched::NO;
}

void Counter::hfReloadState(){
    hfSwitchQuantityCallback((int)conf->hfState.quantity);
    hfSwitchGateTimeCallback(conf->hfState.gateTimeIndexBackup);
}

QString Counter::hfFormatRemainSec(uint countToGo, uint additionTime){
    double timeInSec = (countToGo * (int)conf->hfState.gateTime) / (double)1000 + additionTime;
    QString formatTime;
    if(conf->hfState.gateTime < HFState::GateTime::GATE_TIME_1S)
        formatTime = loc.toString(timeInSec, 'f', 1);
    else
        formatTime = loc.toString(timeInSec, 'd', 0);
    cntWindow->hfSetColorRemainSec(QCOLOR_GREY);
    conf->hfState.hold = HFState::HoldOnState::ON;
    return (formatTime + " Sec ");
}

void Counter::hfDisplayErrors(){
    if(conf->hfState.error == HFState::ErrorType::SIMPLE){
        cntWindow->displayFlagHoldOn(cntWindow->displayHF, true);
        cntWindow->displayHF->displayQerrString(" ");
        cntWindow->displayHF->displayTerrString(" ");
        cntWindow->showPMErrorSigns(cntWindow->displayHF, false);
    }else if (conf->hfState.error == HFState::ErrorType::AVERAGE) {
        if(conf->hfState.hold == HFState::HoldOnState::OFF){
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
    conf->hfState.quantState = HFState::QuantitySwitched::YES;
    conf->hfState.quantity = (HFState::Quantity)index;
    write(cmd->HF_QUANTITY, cmd->pQUANTITIY.at(index));
    cntWindow->clearDisplay(cntWindow->displayHF, true);
}

void Counter::hfSwitchGateTimeCallback(int index){
    conf->hfState.gateTimeIndexBackup = index;
    write(cmd->HF_GATE_TIME, cmd->pHF_GATE_TIME.at(index));

    if(index == 0){
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    }else if(index == 1){
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_500M;
    }else if(index == 2){
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_1S;
    }else if(index == 3){
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_5S;
    }else if(index == 4){
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_10S;
    }
    movAvg->clear();
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayHF->displayAvgString(seconds);
    cntWindow->clearDisplay(cntWindow->displayHF, true);
}

void Counter::hfSwitchErrorAvgCallback(int index){
    conf->hfState.error = (HFState::ErrorType)index;
    hfDisplayErrors();
}

void Counter::hfDialAvgChangedCallback(float val){
    movAvg->setBufferSize(qFloor(val));
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

    if(mode == "FPME"){
        strQerr = loc.toString(qerr, 'g', 3).replace(loc.decimalPoint(), '.');
        strTerr = loc.toString(terr, 'g', 3).replace(loc.decimalPoint(), '.');

        bool isFrequency = (quantity == "QFRE") ? true : false;
        strVal = formatNumber(val1, qerr+terr);

        val1 = (isFrequency) ? val1 : 1 / val1;
        if(isRangeExceeded(val1)){
            cntWindow->displayFlagSwitchMode(display, true);
        }else {
            displayValues(display, strVal, "", strQerr, strTerr);
        }

        display->updateProgressBar(val1);
        cntWindow->showPMErrorSigns(display, true);

    }else if (mode == "DUTY") {
        strVal = loc.toString(val1, 'f', 3).replace(loc.decimalPoint(), '.');
        strVal2 = loc.toString(val2, 'e', 5).replace(loc.decimalPoint(), '.');
        cntWindow->showPMErrorSigns(display, true);
        displayValues(display, strVal, strVal2, "", "");
    }
}

void Counter::lfReloadState(){
    LFState::Channel chan = (conf->lfState.activeChan == LFState::ActiveChan::CHAN1) ? conf->lfState.chan1 : conf->lfState.chan2;
    if (chan.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
        lfSwitchDutyCycleCallback((int)LFState::Channel::DutyCycle::ENABLED);
    }else {
        lfReloadStateQuantMeasurement();
    }
}

void Counter::lfReloadStateQuantMeasurement(){
    lfSwitchQuantity((int)conf->lfState.chan1.quantity, cmd->LF_CH1_QUANTITY);
    lfSwitchQuantity((int)conf->lfState.chan2.quantity, cmd->LF_CH2_QUANTITY);
    lfSwitchMultiplier((int)conf->lfState.chan1.multiplier, cmd->LF_CH1_MULTIPLIER);
    lfSwitchMultiplier((int)conf->lfState.chan2.multiplier, cmd->LF_CH2_MULTIPLIER);
    lfDialSampleCountCh1ChangedCallback(conf->lfState.chan1.sampleCountBackup);
    lfDialSampleCountCh2ChangedCallback(conf->lfState.chan2.sampleCountBackup);
}

bool Counter::isRangeExceeded(double frequency){
    return (frequency > spec->lf_max) ? true : false;
}

void Counter::lfSwitchChannelCallback(int index){
    conf->lfState.activeChan = LFState::ActiveChan(index);
    LFState::Channel chanState = (index == 0) ? conf->lfState.chan1 : conf->lfState.chan2;
    cntWindow->tabLowFreq->buttonsQuantitySwitch->setChecked(true, (int)chanState.quantity);
    cntWindow->tabLowFreq->buttonsDutyCycleSwitch->setChecked(true, (int)chanState.dutyCycle);
    cntWindow->tabLowFreq->buttonsMultiplierSwitch->setChecked(true, (int)chanState.multiplier);
}

void Counter::lfSwitchQuantityCallback(int index){
    QByteArray chanQuant;
    WidgetDisplay *display = cntWindow->displayLFCh1;
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        conf->lfState.chan1.quantity = (LFState::Channel::Quantity)index;
        display = cntWindow->displayLFCh1;
        chanQuant = cmd->LF_CH1_QUANTITY;
    }else if (conf->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        conf->lfState.chan2.quantity = (LFState::Channel::Quantity)index;
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
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        conf->lfState.chan1.multiplier = (LFState::Channel::Multiplier)index;
        multip = cmd->LF_CH1_MULTIPLIER;
    }else if (conf->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        conf->lfState.chan2.multiplier = (LFState::Channel::Multiplier)index;
        multip = cmd->LF_CH2_MULTIPLIER;
    }
    lfSwitchMultiplier(index, multip);
}

void Counter::lfSwitchMultiplier(int index, QByteArray channelMultiplier){
    write(channelMultiplier, cmd->pLF_MULTIPLIER.at(index));
}

void Counter::lfSwitchDutyCycleCallback(int index){
    QByteArray chanDutyCycle;
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        conf->lfState.chan1.dutyCycle = (LFState::Channel::DutyCycle)index;
        chanDutyCycle = cmd->LF_CH1_DUTY_CYCLE_ENABLE;
    }else if (conf->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        conf->lfState.chan2.dutyCycle = (LFState::Channel::DutyCycle)index;
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
    val = conf->lfState.chan1.sampleCountBackup = qFloor(val);
    write(cmd->LF_CH1_SAMPLE_COUNT, val);
}

void Counter::lfDialSampleCountCh2ChangedCallback(float val){
    val = conf->lfState.chan2.sampleCountBackup = qFloor(val);
    write(cmd->LF_CH2_SAMPLE_COUNT, val);
}

/************************************** RATIO MEAS. FUNCTIONS ****************************************/
void Counter::parseRatioCounter(QByteArray data){
    QByteArray warn = data.left(4); data.remove(0, 4);

    double val; QString strVal;
    QDataStream streamBuffLeng(data);
    streamBuffLeng >> val;

    if(warn == "WARN"){
        cntWindow->ratDisplayFlagWarning(cntWindow->displayRat, true);
    }else {
        cntWindow->displayFlagHoldOn(cntWindow->displayRat, false);
        strVal = loc.toString(val, 'f', 5).replace(loc.decimalPoint(), '.');
        cntWindow->displayRat->displayString(strVal);
        /* Print-out the error */
        double error = 1 / conf->ratState.sampleCountBackup;
        strVal = loc.toString(error, 'f', 5).replace(loc.decimalPoint(), '.');
        cntWindow->displayRat->displayTerrString(strVal);
        cntWindow->showPMErrorSigns(cntWindow->displayRat, true);
        cntWindow->displayRat->drawIndicationFlag(LABELNUM_FLAG, "blue");
    }
}

void Counter::ratReloadState(){
    ratDialSampleCountChangedCallback(conf->ratState.sampleCountBackup);
}

void Counter::ratDialSampleCountChangedCallback(float val){
    cntWindow->clearDisplay(cntWindow->displayRat, true);
    val = conf->ratState.sampleCountBackup = qFloor(val);
    write(cmd->RAT_CH3_SAMPLE_COUNT, val);
}

void Counter::ratRetriggerCallback(int index){
    Q_UNUSED(index);
    cntWindow->clearDisplay(cntWindow->displayRat, true);
    write(cmd->RAT_CH3_SAMPLE_COUNT, conf->ratState.sampleCountBackup);
}

/************************************** INTERVALS FUNCTIONS ****************************************/
void Counter::intReloadState()
{

}

void Counter::writeConfiguration(){

}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
