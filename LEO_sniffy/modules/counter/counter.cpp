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
        cntWindow->specReceived(spec); // something like signal :D
    }else {

        if(dataHeader == "HF_D"){
            parseHighFrequencyCounter(dataToPass);
        }else if(dataHeader=="LF_D"){
            parseLowFrequencyCounter(dataToPass);
        }else if(dataHeader=="RF_D"){

        }else if(dataHeader=="TI_D"){

        }
    }
}

/************************************** COMMON FUNCTIONS ****************************************/

QString Counter::formatNumber(double valToFormat, double error)
{
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
    if(conf->mode == CounterMode::HIGH_FREQUENCY)
        display->displayAvgString(avg);
    display->displayString(val);
    display->displayQerrString(qerr);
    display->displayTerrString(terr);
    QString color = (display == cntWindow->displayCh1) ? "blue" : "grey";
    display->drawIndicationFlag(LABELNUM_FLAG, color);
}

void Counter::clearDisplay(WidgetDisplay *display){
    if(conf->hfState.hold == HFState::HoldOnState::OFF)
        display->displayAvgString("");
    display->displayString("");
    display->displayQerrString("");
    display->displayTerrString("");
    cntWindow->displayFlagHoldOn(display, true);
    cntWindow->showPMErrorSigns(display, false);
}

void Counter::switchCounterModeCallback(int index){    
    conf->mode = (CounterMode)index;
    cntWindow->reconfigDisplayLabelArea(spec);

    if(index == 0){
        write(cmd->COUNTER_MODE, cmd->MODE_HIGH_FREQ);

        clearDisplay(cntWindow->displayCh1);
        cntWindow->displayFlagHoldOn(cntWindow->displayCh1, true);

        hfSwitchQuantityCallback((int)conf->hfState.quantity);
        hfSwitchGateTimeCallback(conf->hfState.gateTimeIndexBackup);

    }else if(index == 1){
        write(cmd->COUNTER_MODE, cmd->MODE_LOW_FREQ);

        clearDisplay(cntWindow->displayCh1);
        clearDisplay(cntWindow->displayCh2);
        cntWindow->displayFlagHoldOn(cntWindow->displayCh1, true);
        cntWindow->displayFlagHoldOn(cntWindow->displayCh2, true);

        lfSwitchQuantity((int)conf->lfState.chan1.quantity, cmd->LF_CH1_QUANTITY);
        lfSwitchQuantity((int)conf->lfState.chan2.quantity, cmd->LF_CH2_QUANTITY);
        lfSwitchMultiplier((int)conf->lfState.chan1.multiplier, cmd->LF_CH1_MULTIPLIER);
        lfSwitchMultiplier((int)conf->lfState.chan2.multiplier, cmd->LF_CH2_MULTIPLIER);
        lfDialSampleCountCh1ChangedCallback(conf->lfState.chan1.sampleCountBackup);
        lfDialSampleCountCh2ChangedCallback(conf->lfState.chan2.sampleCountBackup);

    }else if(index == 2){
        write(cmd->COUNTER_MODE, cmd->MODE_REFERENCE);
    }else if(index == 3){
        write(cmd->COUNTER_MODE, cmd->MODE_INTERVAL);
    }
}

void Counter::write(QByteArray feature, QByteArray param){
    comm->write(moduleCommandPrefix, feature, param);
}

//void Counter::write2(QByteArray feature, QByteArray param){
//    comm->write(moduleCommandPrefix, feature, param);
//}

void Counter::write(QByteArray feature, int param){
    comm->write(moduleCommandPrefix, feature, param);
}

//void Counter::msleep(int msec){
//    QEventLoop loop;
//    QTimer::singleShot(msec, &loop, &QEventLoop::quit);
//    loop.exec();
//}

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
    cntWindow->showPMErrorSigns(cntWindow->displayCh1, true);

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
            cntWindow->showPMErrorSigns(cntWindow->displayCh1, false);
            strQerr = " ";
            strTerr = " ";
        }
        strAvg = hfFormatRemainSec(countToGo, 0);
    }

    displayValues(cntWindow->displayCh1, formatNumber(val, qerr+terr), strAvg, strQerr, strTerr);
    cntWindow->displayFlagHoldOn(cntWindow->displayCh1, false);
    val = (isFrequency) ? val : 1 / val;
    cntWindow->displayCh1->updateProgressBar(val);
    conf->hfState.quantState = HFState::QuantitySwitched::NO;
}

void Counter::hfDisplayErrors(){
    if(conf->hfState.error == HFState::ErrorType::AVERAGE){
        if(conf->hfState.hold == HFState::HoldOnState::ON){
            cntWindow->displayCh1->displayQerrString(" ");
            cntWindow->displayCh1->displayTerrString(" ");
            cntWindow->showPMErrorSigns(cntWindow->displayCh1, false);
        }else if (conf->hfState.hold == HFState::HoldOnState::OFF) {
            cntWindow->displayCh1->displayQerrString(avgQerr);
            cntWindow->displayCh1->displayTerrString(strTerr);
            cntWindow->showPMErrorSigns(cntWindow->displayCh1, true);
        }
    }
}

QString Counter::hfFormatRemainSec(uint countToGo, uint additionTime){
    double timeInSec = (countToGo * (int)conf->hfState.gateTime) / 1000 + additionTime;
    QString formatTime = loc.toString(timeInSec, 'd', 0);
    cntWindow->hfSetColorRemainSec(QCOLOR_GREY);
    conf->hfState.hold = HFState::HoldOnState::ON;
    return (formatTime + " Sec ");
}

void Counter::hfSwitchQuantityCallback(int index){
    conf->hfState.quantState = HFState::QuantitySwitched::YES;
    conf->hfState.quantity = (HFState::Quantity)index;

    if(index == 0){
        write(cmd->HF_QUANTITY, cmd->QUANT_FREQUENCY);
    }else{
        write(cmd->HF_QUANTITY, cmd->QUANT_PERIOD);
    }
    clearDisplay(cntWindow->displayCh1);
}

void Counter::hfSwitchGateTimeCallback(int index){
    conf->hfState.gateTimeIndexBackup = index;

    if(index == 0){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_100M);
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    }else if(index == 1){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_500M);
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_500M;
    }else if(index == 2){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_1S);
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_1S;
    }else if(index == 3){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_5S);
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_5S;
    }else if(index == 4){
        write(cmd->HF_GATE_TIME, cmd->HF_TIME_10S);
        conf->hfState.gateTime = HFState::GateTime::GATE_TIME_10S;
    }
    movAvg->clear();
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayCh1->displayAvgString(seconds);
    clearDisplay(cntWindow->displayCh1);
}

void Counter::hfSwitchErrorAvgCallback(int index){
    conf->hfState.error = (HFState::ErrorType)index;
    cntWindow->displayFlagHoldOn(cntWindow->displayCh1, (bool)!index);
    hfDisplayErrors();
}

void Counter::hfDialAvgChangedCallback(float val){
    movAvg->setBufferSize(qFloor(val));
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayCh1->displayAvgString(seconds);
    hfDisplayErrors();
}

/************************************** LOW FREQ FUNCTIONS ****************************************/

void Counter::parseLowFrequencyCounter(QByteArray data){
    QByteArray channel = data.left(4);
    QByteArray freqPer = data.mid(4, 4);
    data.remove(0, 8);

    double val, qerr, terr;
    QString strVal, strQerr, strTerr;
    QDataStream streamBuffLeng(data);
    streamBuffLeng >> val >> qerr >> terr;

    bool isFrequency = (freqPer == "QFRE") ? true : false;
    WidgetDisplay *display = cntWindow->displayCh1;

    if(channel == "IC1D"){
        display = cntWindow->displayCh1;
    }else if (channel == "IC2D"){
        display = cntWindow->displayCh2;
    }

    strVal = formatNumber(val, qerr+terr);
    strQerr = loc.toString(qerr, 'g', 3).replace(loc.decimalPoint(), '.');
    strTerr = loc.toString(terr, 'g', 3).replace(loc.decimalPoint(), '.');

    val = (isFrequency) ? val : 1 / val;
    if(isRangeExceeded(val)){
        cntWindow->displayFlagSwitchMode(display, true);
    }else {
        displayValues(display, strVal, "", strQerr, strTerr);
        cntWindow->displayFlagSwitchMode(display, false);
    }

    display->updateProgressBar(val);
    cntWindow->showPMErrorSigns(display, true);
}

bool Counter::isRangeExceeded(double frequency){
    return (frequency > spec->lf_uppLmt) ? true : false;
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
    WidgetDisplay *display = cntWindow->displayCh1;
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        conf->lfState.chan1.quantity = (LFState::Channel::Quantity)index;
        display = cntWindow->displayCh1;
        chanQuant = cmd->LF_CH1_QUANTITY;
    }else if (conf->lfState.activeChan == LFState::ActiveChan::CHAN2) {
        conf->lfState.chan2.quantity = (LFState::Channel::Quantity)index;
        display = cntWindow->displayCh2;
        chanQuant = cmd->LF_CH2_QUANTITY;
    }
    lfSwitchQuantity(index, chanQuant);
    clearDisplay(display);
}

void Counter::lfSwitchQuantity(int index, QByteArray channelQuantitiy){
    if(index == 0){
        write(channelQuantitiy, cmd->QUANT_FREQUENCY);
    }else if (index == 1) {
        write(channelQuantitiy, cmd->QUANT_PERIOD);
    }
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

//    if(index == 0){
//        write(channelMultiplier, cmd->LF_MULTIPLIER_1X);
//    }else if (index == 1) {
//        write(channelMultiplier, cmd->LF_MULTIPLIER_2X);
//    }else if (index == 2) {
//        write(channelMultiplier, cmd->LF_MULTIPLIER_4X);
//    }else if (index == 3) {
//        write(channelMultiplier, cmd->LF_MULTIPLIER_8X);
//    }
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

    if(index == 0){
        write(cmd->LF_DUTY_CYCLE, cmd->LF_DUTY_CYCLE_DISABLE);
    }else if (index == 1) {
        write(cmd->LF_DUTY_CYCLE, chanDutyCycle);
    }
}

void Counter::lfDialSampleCountCh1ChangedCallback(float val){
    int value;
    value = conf->lfState.chan1.sampleCountBackup = qFloor(val);
    write(cmd->LF_CH1_SAMPLE_COUNT, value);
}

void Counter::lfDialSampleCountCh2ChangedCallback(float val){
    int value;
    value = conf->lfState.chan2.sampleCountBackup = qFloor(val);
    write(cmd->LF_CH2_SAMPLE_COUNT, value);
}

void Counter::writeConfiguration(){

}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
