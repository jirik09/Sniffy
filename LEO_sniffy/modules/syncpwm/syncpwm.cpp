#include "syncpwm.h"

SyncPWM::SyncPWM(QObject *parent)
{
    Q_UNUSED(parent);

    spwmWindow = new SyncPWMWindow();
    moduleName = "Sync PWM";
    moduleIconURI = ":/graphics/graphics/icon_syncpwm.png";

    moduleCommandPrefix = cmd->SYNC_PWM_GEN;
}

void SyncPWM::write(QByteArray feature, QByteArray param){
    comm->write(moduleCommandPrefix, feature, param);
}

void SyncPWM::write(QByteArray feature, int param){
    comm->write(moduleCommandPrefix, feature, param);
}

void SyncPWM::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.remove(0, 4);

    if(dataHeader == "CFG_"){
        moduleSpecification = new SyncPWMSpec(this);
        moduleSpecification->parseSpecification(dataToPass);
        //spwmWindow->setSpecification(static_cast<SyncPWMSpec*>(moduleSpecification));
        showModuleControl();
    }else {

    }
}

void SyncPWM::startModule(){
    write(cmd->SPWM_COMMAND, cmd->SPWM_INIT);
    write(cmd->SPWM_COMMAND, cmd->START);
}

void SyncPWM::stopModule(){

}

void SyncPWM::writeConfiguration(){

}

void SyncPWM::parseConfiguration(QByteArray config){
    Q_UNUSED(config);
}

QByteArray SyncPWM::getConfiguration(){

}

QWidget *SyncPWM::getWidget(){
    return spwmWindow;
}

SyncPWM::~SyncPWM(){

}
