#include "counter.h"

Counter::Counter(QObject *parent)
{
    Q_UNUSED(parent);
    config = new CounterConfig(this);
    cntWindow = new CounterWindow();
    setCommandPrefix(cmd->COUNTER);
    moduleName = "Counter";
}

void Counter::startModule(){

}

void Counter::stopModule(){

}

void Counter::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        showModuleControl();

    }else if(dataHeader=="GATE"){

    }
}

void Counter::writeConfiguration(){

}

QWidget* Counter::getWidget(){
    return cntWindow;
}

Counter::~Counter(){

}
