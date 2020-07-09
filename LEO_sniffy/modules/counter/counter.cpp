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

}

void Counter::stopModule(){

}

void Counter::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CNT_"){ //TODO - tohle by melo byt "CFG_" az budes delat MCU posilani specifikace tak to prepis
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
