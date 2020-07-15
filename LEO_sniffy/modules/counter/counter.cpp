#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);
    config = new CounterConfig(this);
    cntWindow = new CounterWindow();

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

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
    }else if(dataHeader=="ETRD"){        
        cntWindow->getDisplayChannel1()->displayNumber(displayValue);
    }else if(dataHeader=="IC1D"){
        cntWindow->getDisplayChannel1()->displayNumber(displayValue);
    }else if(dataHeader=="IC2D"){
        cntWindow->getDisplayChannel2()->displayNumber(displayValue);
    }
}

void Counter::writeConfiguration(){

}

/*Test function*/
//void Counter::tabBarClicked(int index){
//    if(index == 0){
//        cntWindow->getDisplayChannel1()->displayNumber(11);
//    }else if(index == 1){
//        cntWindow->getDisplayChannel1()->displayNumber(22);
//    }else{
//        cntWindow->getDisplayChannel1()->displayNumber(33);
//    }
//}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
