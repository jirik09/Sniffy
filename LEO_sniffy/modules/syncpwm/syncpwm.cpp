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
    connect(spwmWindow->settings->buttonEquidist, &WidgetButtons::clicked, this, &SyncPwm::buttonEquidistantModeCallback);

    connect(spwmWindow->settings->onOffCh[0], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCh1Callback);
    connect(spwmWindow->settings->onOffCh[1], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCh2Callback);
    connect(spwmWindow->settings->onOffCh[2], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCh3Callback);
    connect(spwmWindow->settings->onOffCh[3], &WidgetSwitch::clicked, this, &SyncPwm::switchOnOffCh4Callback);

    connect(spwmWindow->settings->inverCh[0], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCh1Callback);
    connect(spwmWindow->settings->inverCh[1], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCh2Callback);
    connect(spwmWindow->settings->inverCh[2], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCh3Callback);
    connect(spwmWindow->settings->inverCh[3], &WidgetButtons::clicked, this, &SyncPwm::buttonInvertCh4Callback);

    connect(spwmWindow->settings->dialFreqCh[0], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCh1Callback);
    connect(spwmWindow->settings->dialFreqCh[1], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCh2Callback);
    connect(spwmWindow->settings->dialFreqCh[2], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCh3Callback);
    connect(spwmWindow->settings->dialFreqCh[3], &WidgetDialRange::valueChanged, this, &SyncPwm::dialFreqCh4Callback);

    connect(spwmWindow->settings->dialDutyCh[0], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCh1Callback);
    connect(spwmWindow->settings->dialDutyCh[1], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCh2Callback);
    connect(spwmWindow->settings->dialDutyCh[2], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCh3Callback);
    connect(spwmWindow->settings->dialDutyCh[3], &WidgetDialRange::valueChanged, this, &SyncPwm::dialDutyCh4Callback);

    connect(spwmWindow->settings->dialPhaseCh[0], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCh1Callback);
    connect(spwmWindow->settings->dialPhaseCh[1], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCh2Callback);
    connect(spwmWindow->settings->dialPhaseCh[2], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCh3Callback);
    connect(spwmWindow->settings->dialPhaseCh[3], &WidgetDialRange::valueChanged, this, &SyncPwm::dialPhaseCh4Callback);
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

/****************************** Process Functions ******************************/

void SyncPwm::enableChannel(int chanIndex, bool enable){

}

void SyncPwm::invertChannel(int chanIndex, bool invert){

}

void SyncPwm::setFreqChannel(int chanIndex, float val){

}

void SyncPwm::setDutyChannel(int chanIndex, float val){

}

void SyncPwm::setPhaseChannel(int chanIndex, float val){

}

/****************************** Callbacks ******************************/

void SyncPwm::buttonStartCallback(int index){
    Q_UNUSED(index);
    if(config->state == SyncPwmState::RUNNING){
        spwmWindow->settings->buttonStart->setText("STOP");
        spwmWindow->settings->buttonStart->setChecked(true, 0);
        stop();
    }else {
        spwmWindow->settings->buttonStart->setText("START");
        spwmWindow->settings->buttonStart->setChecked(false, 0);
        start();
    }
}

void SyncPwm::buttonEquidistantModeCallback(int index){
    Q_UNUSED(index);
    if(config->equiMode == SyncPwmModeEqui::EQUI_ENABLED){

    }else {

    }
}

void SyncPwm::switchOnOffCh1Callback(int index){
    bool enable = spwmWindow->settings->onOffCh[0]->isCheckedLeft();
    enableChannel(index, enable);
}

void SyncPwm::switchOnOffCh2Callback(int index){
    bool enable = spwmWindow->settings->onOffCh[1]->isCheckedLeft();
    enableChannel(index, enable);
}

void SyncPwm::switchOnOffCh3Callback(int index){
    bool enable = spwmWindow->settings->onOffCh[2]->isCheckedLeft();
    enableChannel(index, enable);
}

void SyncPwm::switchOnOffCh4Callback(int index){
    bool enable = spwmWindow->settings->onOffCh[3]->isCheckedLeft();
    enableChannel(index, enable);
}

void SyncPwm::buttonInvertCh1Callback(int index){
    bool invert = spwmWindow->settings->inverCh[0]->isChecked(0);
    invertChannel(index, invert);
}

void SyncPwm::buttonInvertCh2Callback(int index){
    bool invert = spwmWindow->settings->inverCh[1]->isChecked(1);
    invertChannel(index, invert);
}

void SyncPwm::buttonInvertCh3Callback(int index){
    bool invert = spwmWindow->settings->inverCh[2]->isChecked(2);
    invertChannel(index, invert);
}

void SyncPwm::buttonInvertCh4Callback(int index){
    bool invert = spwmWindow->settings->inverCh[3]->isChecked(3);
    invertChannel(index, invert);
}

void SyncPwm::dialFreqCh1Callback(float val){
    setFreqChannel(0, val);
}

void SyncPwm::dialFreqCh2Callback(float val){
    setFreqChannel(1, val);
}

void SyncPwm::dialFreqCh3Callback(float val){
    setFreqChannel(2, val);
}

void SyncPwm::dialFreqCh4Callback(float val){
    setFreqChannel(3, val);
}

void SyncPwm::dialDutyCh1Callback(float val){
    setDutyChannel(0, val);
}

void SyncPwm::dialDutyCh2Callback(float val){
    setDutyChannel(1, val);
}

void SyncPwm::dialDutyCh3Callback(float val){
    setDutyChannel(2, val);
}

void SyncPwm::dialDutyCh4Callback(float val){
    setDutyChannel(3, val);
}

void SyncPwm::dialPhaseCh1Callback(float val){
    setPhaseChannel(0, val);
}

void SyncPwm::dialPhaseCh2Callback(float val){
    setPhaseChannel(1, val);
}

void SyncPwm::dialPhaseCh3Callback(float val){
    setPhaseChannel(2, val);
}

void SyncPwm::dialPhaseCh4Callback(float val){
    setPhaseChannel(3, val);
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

SyncPwm::~SyncPwm(){

}
