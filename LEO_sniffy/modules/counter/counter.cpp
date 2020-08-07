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

    connect(cntWindow->tabs, &widgetTab::tabBarClicked, this, &Counter::switchCounterModeCallback);
    connect(cntWindow->tabHighFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &Counter::switchQuantityCallback);
    connect(cntWindow->tabHighFreq->buttonsGateTime, &WidgetButtons::clicked, this, &Counter::switchGateTimeCallback);
    connect(cntWindow->tabHighFreq->buttonsErrorSwitch, &WidgetButtons::clicked, this, &Counter::switchErrorAvgCallback);
    connect(cntWindow->tabHighFreq->dialAveraging, &WidgetDialRange::valueChanged, this, &Counter::dialAvgChangedCallback);
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
    QByteArray meta = data.mid(4, 4);
    QByteArray dataToPass = data.remove(0, 8);

    double val, qerr, terr;
    QString strVal, strQerr, strTerr;

    if(dataHeader == "CFG_"){
        showModuleControl();
        spec = new CounterSpec(dataToPass, this);
        cntWindow->specReceived(spec); // something like signal :D
    }else {
        QDataStream streamBuffLeng(dataToPass);
        streamBuffLeng >> val >> qerr >> terr;
    }

    if(dataHeader == "ETR_"){
        QString strAvg;
        bool isFrequency = (meta == "QFRE") ? true : false;
        uint countToGo = (isFrequency) ? movAvg->prepend(val) : movAvg->prepend(1/val);

        this->strQerr = strQerr = loc.toString(qerr, 'g', 6).replace(loc.decimalPoint(), '.');
        this->strTerr = strTerr = loc.toString(terr, 'g', 6).replace(loc.decimalPoint(), '.');

        if(movAvg->isBufferFull()){
            config->hold = HoldOnState::OFF;
            cntWindow->displayHoldOnFlag(false);
            cntWindow->setColorHoldOn(QCOLOR_WHITE);
            double avg = (isFrequency) ? movAvg->getAverage() : 1 / movAvg->getAverage();
            if (config->error == ErrorType::AVERAGE){
                qerr = (isFrequency) ? qerr / movAvg->getBufferSize() : 1 / (1 / avg - movAvg->getBufferSize()) - avg;
                avgQerr = strQerr = loc.toString(qerr, 'g', 6).replace(loc.decimalPoint(), '.');
            }
            strAvg = formatNumber(avg, qerr+terr);
        }else {
            if(config->error == ErrorType::AVERAGE){
                strQerr = " ";
                strTerr = " ";
            }
            strAvg = formatHoldOn(countToGo, 0);
        }

        displayValues(cntWindow->displayCh1, formatNumber(val, qerr+terr), strAvg, strQerr, strTerr);
        val = (isFrequency) ? val : 1 / val;
        cntWindow->displayCh1->updateProgressBar(val);
        config->quantState = QuantitySwitched::NO;

    }else if(dataHeader=="IC1D"||dataHeader=="IC2D"){

        if(dataHeader=="IC1D"){
            strVal = loc.toString(val, ' ', 6);
            cntWindow->displayCh1->drawFlagLabel(4);
            cntWindow->displayCh1->displayString(strVal);
        }else {
            strVal = loc.toString(val, ' ', 6);
            cntWindow->displayCh2->drawFlagLabel(4);
            cntWindow->displayCh2->displayString(strVal);
        }

    }else if(dataHeader=="REFD"){

    }else if(dataHeader=="TIDA"){

    }
}

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

QString Counter::formatHoldOn(uint countToGo, uint additionTime)
{
    double timeInSec = (countToGo * config->gateTime) / 1000 + additionTime;
    QString formatTime = loc.toString(timeInSec, 'd', 0);

//    if(timeInSec == 0){ timeInSec = 1; }
//    int digitNum = (timeInSec < 1000) ? (int)(log10(timeInSec) + 1) : (int)(log10(timeInSec) + 2);

//    for(int i = 0; i < 5 - digitNum; i++){
//        formatTime.prepend(' ');
//    }

    cntWindow->setColorHoldOn(QCOLOR_GREY);  // QColor(138,110,53) QColor(152,123,67));
    cntWindow->displayHoldOnFlag(true);
    config->hold = HoldOnState::ON;

    //return ("HOLd " + formatTime + "  Sec");
    return (formatTime + " Sec ");
}

void Counter::switchCounterModeCallback(int index){
    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_HIGH_FREQ);
        config->mode = CounterMode::HIGH_FREQUENCY;
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_LOW_FREQ);
        config->mode = CounterMode::LOW_FREQUENCY;
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_REFERENCE);
        config->mode = CounterMode::REFERENCE;
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_INTERVAL);
        config->mode = CounterMode::INTERVAL;
    }
}

void Counter::switchQuantityCallback(int index){
    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_QUANTITY, cmd->QUANT_FREQUENCY);
        config->quantity = CounterQuantity::FREQUENCY;
    }else{
        comm->write(cmd->COUNTER, cmd->COUNTER_QUANTITY, cmd->QUANT_PERIOD);
        config->quantity = CounterQuantity::PERIOD;
    }
    config->quantState = QuantitySwitched::YES;
    cntWindow->displayHoldOnFlag(true);
    clearDisplay(cntWindow->displayCh1);
}

void Counter::switchGateTimeCallback(int index){
    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_100M);
        config->gateTimeSel = GateTime::GATE_TIME_100M;
        config->gateTime = 100;
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_500M);
        config->gateTimeSel = GateTime::GATE_TIME_500M;
        config->gateTime = 500;
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_1S);
        config->gateTimeSel = GateTime::GATE_TIME_1S;
        config->gateTime = 1000;
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_5S);
        config->gateTimeSel = GateTime::GATE_TIME_5S;
        config->gateTime = 5000;
    }else if(index == 4){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_10S);
        config->gateTimeSel = GateTime::GATE_TIME_10S;
        config->gateTime = 10000;
    }
    QString holdOn = formatHoldOn(movAvg->getSampleCountToFillBuff(), config->gateTime/1000);
    cntWindow->displayCh1->displayAvgString(holdOn);
    displayErrors(cntWindow->displayCh1);
}

void Counter::switchErrorAvgCallback(int index){
    config->error = (index == 0) ? ErrorType::SIMPLE : ErrorType::AVERAGE;
    displayErrors(cntWindow->displayCh1);
}

void Counter::dialAvgChangedCallback(float val){
    movAvg->setBufferSize(qFloor(val));
    QString holdOn = formatHoldOn(movAvg->getSampleCountToFillBuff(), 0);
    cntWindow->displayCh1->displayAvgString(holdOn);
    displayErrors(cntWindow->displayCh1);
}

void Counter::displayErrors(WidgetDisplay *display){
    if(config->error == ErrorType::AVERAGE && config->hold == HoldOnState::ON){
        display->displayQerrString(" ");
        display->displayTerrString(" ");
    }else if (config->error == ErrorType::AVERAGE && config->hold == HoldOnState::OFF) {
        display->displayQerrString(avgQerr);
        display->displayTerrString(strTerr);
    }else if(config->error == ErrorType::SIMPLE && config->quantState == QuantitySwitched::NO) {
        display->displayQerrString(strQerr);
        display->displayTerrString(strTerr);
    }
}

void Counter::displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr){
    display->displayString(val);
    display->displayAvgString(avg);
    display->displayQerrString(qerr);
    display->displayTerrString(terr);
    display->drawFlagLabel(4);
}

void Counter::clearDisplay(WidgetDisplay *display){
    if(config->hold == HoldOnState::OFF){
        display->displayAvgString("");
    }
    display->displayString("");
    display->displayQerrString("");
    display->displayTerrString("");
}

void Counter::writeConfiguration(){

}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
