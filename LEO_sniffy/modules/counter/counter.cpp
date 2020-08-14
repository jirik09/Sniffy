#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);

    loc = QLocale(QLocale::English);

    cntWindow = new CounterWindow();
    config = new CounterConfig(this);

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
    QByteArray dataHeader = data.left(4), metaData = data.mid(4, 4);
    QByteArray dataToPass = data.remove(0, 8);

    if(dataHeader == "CFG_"){
        showModuleControl();
        spec = new CounterSpec(dataToPass, this);
        cntWindow->specReceived(spec); // something like signal :D
    }else {

        if(dataHeader == "ETR_"){
            parseHighFrequencyCounter(metaData, dataToPass);
        }else if(dataHeader=="IC1D"||dataHeader=="IC2D"){

        }else if(dataHeader=="REFD"){

        }else if(dataHeader=="TIDA"){

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
    display->displayString(val);
    display->displayAvgString(avg);
    display->displayQerrString(qerr);
    display->displayTerrString(terr);
    display->drawIndicationFlag(LABELNUM_FLAG);
}

void Counter::clearDisplay(WidgetDisplay *display){
    if(config->hold == StatesHF::HoldOnState::OFF){
        display->displayAvgString("");
    }
    display->displayString("");
    display->displayQerrString("");
    display->displayTerrString("");
    cntWindow->displayHoldOnFlag(display, true);
    cntWindow->showPMErrorSigns(display, false);
}

void Counter::switchCounterModeCallback(int index){
    cntWindow->reconfigDisplayLabelArea(config, spec);
    config->mode = (CounterMode)index;

    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_HIGH_FREQ);
        hfSwitchQuantityCallback((int)config->quantity);
        hfSwitchGateTimeCallback(config->statesHF.gateTimeBackupIndex);
        cntWindow->displayHoldOnFlag(cntWindow->displayCh1, true);

    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_LOW_FREQ);
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_REFERENCE);
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_INTERVAL);
    }
}

/************************************** HIGH FREQ FUNCTIONS ****************************************/

void Counter::parseHighFrequencyCounter(QByteArray metaData, QByteArray data){
    double val, qerr, terr;
    QString strVal, strAvg, strQerr, strTerr;
    QDataStream streamBuffLeng(data);
    streamBuffLeng >> val >> qerr >> terr;

    bool isFrequency = (metaData == "QFRE") ? true : false;
    uint countToGo = (isFrequency) ? movAvg->prepend(val) : movAvg->prepend(1/val);
    this->strQerr = strQerr = loc.toString(qerr, 'g', 6).replace(loc.decimalPoint(), '.');
    this->strTerr = strTerr = loc.toString(terr, 'g', 6).replace(loc.decimalPoint(), '.');
    cntWindow->showPMErrorSigns(cntWindow->displayCh1, true);

    if(movAvg->isBufferFull()){
        config->hold = StatesHF::HoldOnState::OFF;
        cntWindow->hfSetColorRemainSec(QCOLOR_WHITE);
        double avg = (isFrequency) ? movAvg->getAverage() : 1 / movAvg->getAverage();
        if (config->error == StatesHF::ErrorType::AVERAGE){
            qerr = (isFrequency) ? qerr / movAvg->getBufferSize() : 1 / (1 / avg - movAvg->getBufferSize()) - avg;
            avgQerr = strQerr = loc.toString(qerr, 'g', 6).replace(loc.decimalPoint(), '.');
        }
        strAvg = formatNumber(avg, qerr+terr);
    }else {
        if(config->error == StatesHF::ErrorType::AVERAGE){
            cntWindow->showPMErrorSigns(cntWindow->displayCh1, false);
            strQerr = " ";
            strTerr = " ";
        }
        strAvg = hfFormatRemainSec(countToGo, 0);
    }

    displayValues(cntWindow->displayCh1, formatNumber(val, qerr+terr), strAvg, strQerr, strTerr);
    cntWindow->displayHoldOnFlag(cntWindow->displayCh1, false);
    val = (isFrequency) ? val : 1 / val;
    cntWindow->displayCh1->updateProgressBar(val);
    config->quantState = StatesHF::QuantitySwitched::NO;
}

QString Counter::hfFormatRemainSec(uint countToGo, uint additionTime)
{
    double timeInSec = (countToGo * (int)config->gateTime) / 1000 + additionTime;
    QString formatTime = loc.toString(timeInSec, 'd', 0);

    //    if(timeInSec == 0){ timeInSec = 1; }
    //    int digitNum = (timeInSec < 1000) ? (int)(log10(timeInSec) + 1) : (int)(log10(timeInSec) + 2);

    //    for(int i = 0; i < 5 - digitNum; i++){
    //        formatTime.prepend(' ');
    //    }

    cntWindow->hfSetColorRemainSec(QCOLOR_GREY);  // QColor(138,110,53) QColor(152,123,67));
    //cntWindow->displayHoldOnFlag(cntWindow->displayCh1, true);
    config->hold = StatesHF::HoldOnState::ON;

    //return ("HOLd " + formatTime + "  Sec");
    return (formatTime + " Sec ");
}

void Counter::hfSwitchQuantityCallback(int index){
    config->quantState = StatesHF::QuantitySwitched::YES;
    config->quantity = (StatesHF::Quantity)index;

    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_QUANTITY, cmd->QUANT_FREQUENCY);
    }else{
        comm->write(cmd->COUNTER, cmd->COUNTER_QUANTITY, cmd->QUANT_PERIOD);
    }
    clearDisplay(cntWindow->displayCh1);
}

void Counter::hfSwitchGateTimeCallback(int index){
    config->statesHF.gateTimeBackupIndex = index;

    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_100M);
        config->gateTime = StatesHF::GateTime::GATE_TIME_100M;
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_500M);
        config->gateTime = StatesHF::GateTime::GATE_TIME_500M;
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_1S);
        config->gateTime = StatesHF::GateTime::GATE_TIME_1S;
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_5S);
        config->gateTime = StatesHF::GateTime::GATE_TIME_5S;
    }else if(index == 4){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_10S);
        config->gateTime = StatesHF::GateTime::GATE_TIME_10S;
    }
    movAvg->clearBuffer();
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayCh1->displayAvgString(seconds);
    clearDisplay(cntWindow->displayCh1);
}

void Counter::hfSwitchErrorAvgCallback(int index){ 
    config->error = (StatesHF::ErrorType)index;

    cntWindow->displayHoldOnFlag(cntWindow->displayCh1, (bool)!index);
    hfDisplayErrors();
}

void Counter::hfDialAvgChangedCallback(float val){
    config->statesHF.avgBackupSampleCount = qFloor(val);
    movAvg->setBufferSize(qFloor(val));
    QString seconds = hfFormatRemainSec(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayCh1->displayAvgString(seconds);
    hfDisplayErrors();
}

void Counter::hfDisplayErrors(){    
    if(config->error == StatesHF::ErrorType::AVERAGE){
        if(config->hold == StatesHF::HoldOnState::ON){
            cntWindow->displayCh1->displayQerrString(" ");
            cntWindow->displayCh1->displayTerrString(" ");
            cntWindow->showPMErrorSigns(cntWindow->displayCh1, false);
        }else if (config->hold == StatesHF::HoldOnState::OFF) {
            cntWindow->displayCh1->displayQerrString(avgQerr);
            cntWindow->displayCh1->displayTerrString(strTerr);
            cntWindow->showPMErrorSigns(cntWindow->displayCh1, true);
        }
    }
}

void Counter::writeConfiguration(){

}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
