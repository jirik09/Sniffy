#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);

    QLocale::setDefault(QLocale::English);

    cntWindow = new CounterWindow();
    config = new CounterConfig(this);

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

    movAvg = new MovingAverage(2);

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
    QDataStream streamBuffLeng(data.remove(0, 4));

    double val, qerr, terr;   // Display the Value, Quantization and Timebase errors
    streamBuffLeng >> val;
    streamBuffLeng >> qerr;
    streamBuffLeng >> terr;

    QString strVal, strQerr, strTerr;
    QLocale loc;

    if(dataHeader=="CFG_"){
        showModuleControl();
        //todo pass specification into counterSpec.cpp and parse it

    }else if(dataHeader=="ETRD"){

        double avg;
        strTerr = loc.toString(terr, 'e', 4);
        strQerr = loc.toString(qerr, 'e', 4);

        if(isFrequency()){
            uint countToGo = movAvg->prepend(val);

            if(movAvg->isBufferFull()){
                avg = movAvg->getAverage();
                QString strAvg = loc.toString(avg, 'e', 4);
                cntWindow->getDispChan1()->displayAvgString(strAvg);
                if (config->error == ErrorType::AVERAGE){
                    qerr = qerr / movAvg->getBufferSize();
                    strQerr = loc.toString(qerr, 'e', 4);
                }
            }else {
                double time = countToGo * config->gateTime;
                time = (config->gateTime >= 5000) ? time / 1000 : time / 1000 + 1;
                QString strTime = loc.toString(time, ' ', 0);
                cntWindow->getDispChan1()->displayAvgString("HOLd " + strTime + " Sec");
                if(config->error == ErrorType::AVERAGE){
                    strQerr = " ";
                    strTerr = " ";
                }
            }

        }else if(!isFrequency()){
            uint countToGo = movAvg->prepend(1/val);

            if(movAvg->isBufferFull()){
                avg = 1 / movAvg->getAverage();
                QString strAvg = loc.toString(avg, 'e');
                cntWindow->getDispChan1()->displayAvgString(strAvg);
                if (config->error == ErrorType::AVERAGE){
                    qerr = 1 / (avg - movAvg->getBufferSize()) - 1 / avg;
                    strQerr = loc.toString(qerr, 'e', 4);
                }
            }else {
                double time = countToGo * config->gateTime;
                time = (config->gateTime >= 5000) ? time / 1000 : time / 1000 + 1;
                QString strTime = loc.toString(time, ' ', 0);
                cntWindow->getDispChan1()->displayAvgString("HOLd " + strTime + " Sec");
                if(config->error == ErrorType::AVERAGE){
                    strQerr = " ";
                    strTerr = " ";
                }
            }
        }

        strVal = loc.toString(val, ' ', 4);
        cntWindow->getDispChan1()->displayString(strVal);
        cntWindow->getDispChan1()->displayQerrString(strQerr);
        cntWindow->getDispChan1()->displayTerrString(strTerr);

    }else if(dataHeader=="IC1D"||dataHeader=="IC2D"){

        if(dataHeader=="IC1D"){
            strVal = loc.toString(val, ' ', 6);
            cntWindow->getDispChan1()->displayString(strVal);
        }else {
            strVal = loc.toString(val, ' ', 6);
            cntWindow->getDispChan2()->displayString(strVal);
        }

    }else if(dataHeader=="REFD"){

    }else if(dataHeader=="TIDA"){

    }
}

void Counter::switchCounterModeCallback(int index){
    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_HIGH_FREQ);
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_LOW_FREQ);
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_REFERENCE);
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_MODE, cmd->MODE_INTERVAL);
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
}

void Counter::switchGateTimeCallback(int index){
    if(index == 0){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_100M);
        config->gateTime = 100;
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_500M);
        config->gateTime = 500;
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_1S);
        config->gateTime = 1000;
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_5S);
        config->gateTime = 5000;
    }else if(index == 4){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_10S);
        config->gateTime = 10000;
    }
}

void Counter::switchErrorAvgCallback(int index){
    if(index == 0){
        config->error = ErrorType::SIMPLE;
    }else if(index == 1){
        config->error = ErrorType::AVERAGE;
    }
}

void Counter::dialAvgChangedCallback(float val){
    movAvg->setBufferSize(qCeil(val));
}

bool Counter::isFrequency(){
    bool isFrequency = (config->quantity == CounterQuantity::FREQUENCY) ? true : false;
    return isFrequency;
}

void Counter::writeConfiguration(){

}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
