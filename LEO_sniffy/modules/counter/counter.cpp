#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);
    config = new CounterConfig(this);
    cntWindow = new CounterWindow();

    moduleCommandPrefix = cmd->COUNTER;
    moduleName = "Counter";
    moduleIconURI = ":/graphics/graphics/icon_counter.png";

    connect(cntWindow->getTabs(), &widgetTab::tabBarClicked, this, &Counter::tabBarClickedCallback);

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
void Counter::tabBarClickedCallback(int index){
    if(index == 0){
        /* works well */
    }else if(index == 1){
        /* works well */
    }else{

    }
}

QWidget *Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
