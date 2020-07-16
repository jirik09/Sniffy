#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);
    config = new CounterConfig(this);
    cntWindow = new CounterWindow();

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

    connect(cntWindow->tabs, &widgetTab::tabBarClicked, this, &Counter::switchCounterModeCallback);
    connect(cntWindow->tabHighFreq->buttonsErrorSwitch, &WidgetButtons::clicked, this, &Counter::switchGateTimeCallback);
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
    double displayValue;
    streamBuffLeng >> displayValue;

    if(dataHeader=="CFG_"){
        showModuleControl();
        //todo pass specification into counterSpec.cpp and parse it

    }else if(dataHeader=="ETRD"||dataHeader=="IC1D"||dataHeader=="REFD"||dataHeader=="TIDA"){
        cntWindow->getDisplayChannel1()->displayNumber(displayValue);
    }else if(dataHeader=="IC2D"){
        cntWindow->getDisplayChannel2()->displayNumber(displayValue);
    }
}

void Counter::writeConfiguration(){

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

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
