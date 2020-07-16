#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);
    config = new CounterConfig(this);
    cntWindow = new CounterWindow();

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

    connect(cntWindow->tabs, &widgetTab::tabBarClicked, this, &Counter::switchCounterCallback);
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

    }else if(dataHeader=="ETRD"||"IC1D"||"REFD"||"TIDA"){
        cntWindow->getDisplayChannel1()->displayNumber(displayValue);
    }else if(dataHeader=="IC2D"){
        cntWindow->getDisplayChannel2()->displayNumber(displayValue);
    }

}

void Counter::writeConfiguration(){

}

void Counter::switchCounterCallback(int index){
    if(index == 0){
        comm->write(cmd->COUNTER, cmd->MODE_HIGH_FREQ);
    }else if(index == 1){
        comm->write(cmd->COUNTER, cmd->MODE_LOW_FREQ);
    }else if(index == 2){
        comm->write(cmd->COUNTER, cmd->MODE_REFERENCE);
    }else if(index == 3){
        comm->write(cmd->COUNTER, cmd->MODE_INTERVAL);
    }
}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
