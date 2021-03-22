#include "syncpwm.h"

SyncPwm::SyncPwm(QObject *parent)
{
    Q_UNUSED(parent);

    config = new SyncPwmConfig(this);
    spwmWindow = new SyncPwmWindow(config);

    moduleCommandPrefix = cmd->SYNC_PWM_GEN;
    moduleName = "Sync PWM";
    moduleIconURI = ":/graphics/graphics/icon_syncpwm.png";

    connect(spwmWindow->settings->buttonStart, &WidgetButtons::clicked, this, &SyncPwm::buttonStartCallback);
    connect(spwmWindow->settings->switchStepMode, &WidgetSwitch::clicked, this, &SyncPwm::switchStepModeCallback);
    connect(spwmWindow->settings->buttonEquidist, &WidgetButtons::clicked, this, &SyncPwm::buttonEquidistantModeCallback);

    connect(spwmWindow->settings->onOffCh[0], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCallback);
    connect(spwmWindow->settings->onOffCh[1], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCallback);
    connect(spwmWindow->settings->onOffCh[2], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCallback);
    connect(spwmWindow->settings->onOffCh[3], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCallback);

    connect(spwmWindow->settings->inverCh[0], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCallback);
    connect(spwmWindow->settings->inverCh[1], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCallback);
    connect(spwmWindow->settings->inverCh[2], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCallback);
    connect(spwmWindow->settings->inverCh[3], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCallback);

    connect(spwmWindow->settings->dialFreqCh[0], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCallback);
    connect(spwmWindow->settings->dialFreqCh[1], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCallback);
    connect(spwmWindow->settings->dialFreqCh[2], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCallback);
    connect(spwmWindow->settings->dialFreqCh[3], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCallback);

    connect(spwmWindow->settings->dialDutyCh[0], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCallback);
    connect(spwmWindow->settings->dialDutyCh[1], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCallback);
    connect(spwmWindow->settings->dialDutyCh[2], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCallback);
    connect(spwmWindow->settings->dialDutyCh[3], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCallback);

    connect(spwmWindow->settings->dialPhaseCh[0], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCallback);
    connect(spwmWindow->settings->dialPhaseCh[1], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCallback);
    connect(spwmWindow->settings->dialPhaseCh[2], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCallback);
    connect(spwmWindow->settings->dialPhaseCh[3], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCallback);
}

void SyncPwm::startModule(){
    write(cmd->SPWM_COMMAND, cmd->SPWM_INIT);
    start();
}

void SyncPwm::stopModule(){
    stop();
    // write(cmd->SPWM_COMMAND, cmd->SPWM_DEINIT);
}

void SyncPwm::start(){
    write(cmd->SPWM_COMMAND, cmd->START);
}

void SyncPwm::stop(){
    //   write(cmd->SPWM_COMMAND, cmd->STOP);
}

void SyncPwm::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.remove(0, 4);

    if(dataHeader == "CFG_"){
        moduleSpecification = new SyncPwmSpec(this);
        moduleSpecification->parseSpecification(dataToPass);
        //spwmWindow->setSpecification(static_cast<SyncPWMSpec*>(moduleSpecification));
        showModuleControl();
    }else {

    }
}

void SyncPwm::writeConfiguration(){

}

void SyncPwm::parseConfiguration(QByteArray config){
    Q_UNUSED(config);
}

QByteArray SyncPwm::getConfiguration(){

}

/****************************** Callbacks ******************************/

void SyncPwm::buttonStartCallback(int index){
    Q_UNUSED(index);
    if(config->state == State::RUNNING){
        spwmWindow->settings->buttonStart->setText("STOP");
        stop();
    }else {
        spwmWindow->settings->buttonStart->setText("START");
        start();
    }
}

void SyncPwm::switchStepModeCallback(int index){
    if((!(bool)index))
        write(cmd->SPWM_STEP_MODE, cmd->SPWM_STEP_DISABLE);
    else
        write(cmd->SPWM_STEP_MODE, cmd->SPWM_STEP_ENABLE);
}

void SyncPwm::buttonEquidistantModeCallback(int index){
    Q_UNUSED(index);
    if(config->common.equiMode == true){
        /*TODO: equi calculation and callbacks call */
    }else {

    }
}

void SyncPwm::switchOnOffCallback(int index, int chanIndex){
    write(cmd->SPWM_CHANNEL_STATE, (chanIndex+1)<<8 | !(bool)index);
}

void SyncPwm::buttonInvertCallback(int index, int chanIndex){
    Q_UNUSED(index);
    bool invert = spwmWindow->settings->inverCh[chanIndex]->isChecked(0);
    write(cmd->SPWM_CHANNEL_INVERT, (chanIndex+1)<<8 | invert);
}

void SyncPwm::dialFreqCallback(float val, int chanIndex){
    /*TODO: based on chan_dependency grey out dialFreq channel 2 and 4 */
    //    if(static_cast<SyncPwmSpec*>(moduleSpecification)->chan_dependency){

    //    }
    QByteArray freq = QByteArray::fromRawData(reinterpret_cast<char *>(&val), sizeof(float));

    switch(chanIndex){
    case 0:
        comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_FREQ_CONFIG+":"+cmd->CHANNELS_1+":"+freq+";");
        break;
    case 1:
        comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_FREQ_CONFIG+":"+cmd->CHANNELS_2+":"+freq+";");
        break;
    case 2:
        comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_FREQ_CONFIG+":"+cmd->CHANNELS_3+":"+freq+";");
        break;
    case 3:
        comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_FREQ_CONFIG+":"+cmd->CHANNELS_4+":"+freq+";");
        break;
    }

}

void SyncPwm::dialDutyCallback(float val, int chanIndex){
    QByteArray dutyCycle = QByteArray::fromRawData(reinterpret_cast<char *>(&val), sizeof(float));
    QByteArray phase = QByteArray::fromRawData(reinterpret_cast<char *>(&config->chan[chanIndex].phase), sizeof(float));
    QByteArray idx = QByteArray::fromRawData(reinterpret_cast<char *>(&chanIndex), sizeof(int));
    comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_DUTYPHASE_CONFIG+":"+dutyCycle+":"+phase+":"+idx);
}

void SyncPwm::dialPhaseCallback(float val, int chanIndex){

}

void SyncPwm::write(QByteArray feature, QByteArray param){
    comm->write(moduleCommandPrefix, feature, param);
}

void SyncPwm::write(QByteArray feature, int param){
    comm->write(moduleCommandPrefix, feature, param);
}

QWidget *SyncPwm::getWidget(){
    return spwmWindow;
}

SyncPwm::~SyncPwm(){}
