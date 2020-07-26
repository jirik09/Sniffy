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

    connect(cntWindow->tabs, &widgetTab::tabBarClicked, this, &Counter::switchCounterModeCallback);
    connect(cntWindow->tabHighFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &Counter::switchQuantityCallback);
    connect(cntWindow->tabHighFreq->buttonsGateTime, &WidgetButtons::clicked, this, &Counter::switchGateTimeCallback);
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

    double displayData, displayQerror, displayTerror;   // Display Quantization and Timebase errors
    streamBuffLeng >> displayData;
    streamBuffLeng >> displayQerror;
    streamBuffLeng >> displayTerror;

    QLocale locale;
    QString displayValue;

    if(dataHeader=="CFG_"){
        showModuleControl();
        //todo pass specification into counterSpec.cpp and parse it

    }else if(dataHeader=="ETRD"){
        if(config->quantity == CounterQuantity::FREQUENCY){
            displayValue = locale.toString(displayData, ' ', 4);
            cntWindow->getDisplayChannel1()->displayNumber(displayValue);
        }else {
            displayValue = locale.toString(displayData, 'e');
            cntWindow->getDisplayChannel1()->displayNumber(displayValue);
        }
        displayValue = locale.toString(displayQerror, 'e', 4);
        cntWindow->getDisplayChannel1()->displayQuantErrNumber(displayValue);
        displayValue = locale.toString(displayTerror, 'e', 4);
        cntWindow->getDisplayChannel1()->displayTimebaseErrNumber(displayValue);

    }else if(dataHeader=="IC1D"||dataHeader=="IC2D"){
        if(dataHeader=="IC1D"){
            displayValue = locale.toString(displayData, ' ', 6);
            cntWindow->getDisplayChannel1()->displayNumber(displayValue);
        }else {
            displayValue = locale.toString(displayData, ' ', 6);
            cntWindow->getDisplayChannel2()->displayNumber(displayValue);
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
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_500M);
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_1S);
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_5S);
    }else if(index == 4){
        comm->write(cmd->COUNTER, cmd->COUNTER_GATE, cmd->GATE_TIME_10S);
    }
}

void Counter::writeConfiguration(){

}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
