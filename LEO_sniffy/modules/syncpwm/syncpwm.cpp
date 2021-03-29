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
}

void SyncPwm::stopModule(){
    stop();
    write(cmd->SPWM_COMMAND, cmd->SPWM_DEINIT);
}

void SyncPwm::start(){
    write(cmd->SPWM_COMMAND, cmd->START);
}

void SyncPwm::stop(){
    write(cmd->SPWM_COMMAND, cmd->STOP);
}

void SyncPwm::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.remove(0, 4);

    if(dataHeader == "CFG_"){
        moduleSpecification = new SyncPwmSpec(this);
        moduleSpecification->parseSpecification(dataToPass);
        spwmWindow->setSpecification(static_cast<SyncPwmSpec*>(moduleSpecification));
        showModuleControl();
    }else {
        if(dataHeader == "SPPE"){
            stepGenEndNotif();
        }else if (dataHeader == "SPRF"){
            int chanIndex = data.remove(0, 4).toUInt();

        }
    }
}

void SyncPwm::writeConfiguration(){
    spwmWindow->restoreGUIAfterStartup();

    write(cmd->SPWM_COMMAND, cmd->SPWM_INIT);

    if(config->common.stepMode)
        switchStepModeCallback(1);

    for(int ch = 0; ch < CHANNELS_NUM; ch++){
        switchOnOffCallback(config->chan[ch].enabled, ch);
        buttonInvertCallback(ch, ch);
        dialFreqCallback(config->chan[ch].frequency, ch);
        dialDutyCallback(config->chan[ch].dutyCycle, ch);
    }
}

void SyncPwm::parseConfiguration(QByteArray config){
    Q_UNUSED(config);
    this->config->parse(config);
}

QByteArray SyncPwm::getConfiguration(){
    return config->serialize();
}

/****************************** Callbacks ******************************/
void SyncPwm::stepGenEndNotif(){
    spwmWindow->setStartTxt();
    spwmWindow->uncheckStartButton();
    config->state = State::STOPPED;
}

void SyncPwm::buttonStartCallback(int index){
    Q_UNUSED(index);
    if(config->state == State::RUNNING/*!spwmWindow->settings->buttonStart->isChecked(0)*/){
        stop();
        spwmWindow->setStartTxt();
        config->state = State::STOPPED;
    }else {
        start();
        spwmWindow->setStopTxt();
        config->state = State::RUNNING;
    }
}

void SyncPwm::switchStepModeCallback(int index){
    if((!(bool)index)) {
        config->common.stepMode = false;
        write(cmd->SPWM_STEP_MODE, cmd->SPWM_STEP_DISABLE);
    }else {
        config->common.stepMode = true;
        write(cmd->SPWM_STEP_MODE, cmd->SPWM_STEP_ENABLE);
    }
    stepGenEndNotif();
}

void SyncPwm::buttonEquidistantModeCallback(int index){
    Q_UNUSED(index);
    if(config->common.equiMode){
        config->common.equiMode = false;
    }else {
        config->common.equiMode = true;
    }
}

void SyncPwm::switchOnOffCallback(int index, int chanIndex){
    config->chan[chanIndex].enabled = !(bool)index;
    write(cmd->SPWM_CHANNEL_STATE, (chanIndex)<<8 | config->chan[chanIndex].enabled);
}

void SyncPwm::buttonInvertCallback(int index, int chanIndex){
    Q_UNUSED(index);
    config->chan[chanIndex].inverted = spwmWindow->settings->inverCh[chanIndex]->isChecked(0);
    write(cmd->SPWM_CHANNEL_INVERT, (chanIndex)<<8 | config->chan[chanIndex].inverted);
}

void SyncPwm::dialFreqCallback(float val, int chanIndex){
    config->chan[chanIndex].frequency = val;
    quint32 freq = (quint32)(val * FREQ_PRECISION);
    QByteArray data = intToSend(freq)+":"+intToSend(FREQ_PRECISION)+":"+intToSend(chanIndex);
    comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_FREQ_CONFIG+":"+data+";");

    if(config->common.equiMode && chanIndex == 0)
        equiModeSetFreq(val);
}

void SyncPwm::dialPhaseCallback(float val, int chanIndex){
    config->chan[chanIndex].phase = val;
    quint32 phase = (quint32)(val * PHASE_PRECISION);
    quint32 dutyCycle = (quint32)(config->chan[chanIndex].dutyCycle * DUTY_PRECISION);
    setDutyPhase(dutyCycle, phase, chanIndex);

    if(config->common.equiMode && chanIndex == 0)
        equiModeCalcAndSetPhase(val);
}

void SyncPwm::dialDutyCallback(float val, int chanIndex){
    config->chan[chanIndex].dutyCycle = val;
    quint32 dutyCycle = (quint32)(val * DUTY_PRECISION);
    quint32 phase = (quint32)(config->chan[chanIndex].phase * PHASE_PRECISION);
    setDutyPhase(dutyCycle, phase, chanIndex);

    if(config->common.equiMode && chanIndex == 0)
        equiModeCalcAndSetDuty(val);
}

void SyncPwm::setDutyPhase(quint32 dutyCycle, quint32 phase, quint32 chanIndex){
    comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_DUTYPHASE_CONFIG
                +":"+intToSend(dutyCycle)
                +":"+intToSend(DUTY_PRECISION)
                +":"+intToSend(phase)
                +":"+intToSend(PHASE_PRECISION)
                +":"+intToSend(chanIndex)+";");
}

void SyncPwm::equiModeSetFreq(float val){
    dialFreqCallback(val, 1);
    dialFreqCallback(val, 2);
    dialFreqCallback(val, 3);
}

void SyncPwm::equiModeCalcAndSetPhase(float val){
    dialPhaseCallback(val*2, 1);
    dialPhaseCallback(val*3, 2);
    dialPhaseCallback(val*4, 3);
}

void SyncPwm::equiModeCalcAndSetDuty(float val){
    dialDutyCallback(val, 1);
    dialDutyCallback(val, 2);
    dialDutyCallback(val, 3);
}

void SyncPwm::write(QByteArray feature, QByteArray param){
    comm->write(moduleCommandPrefix, feature, param);
}

void SyncPwm::write(QByteArray feature, int param){
    comm->write(moduleCommandPrefix, feature, param);
}

QByteArray SyncPwm::intToSend(int param){
    char tmp[4] = {0};
    tmp[3] = (param>>24);
    tmp[2] = (param>>16);
    tmp[1] = (param>>8);
    tmp[0] = (param);

    return QByteArray(tmp,4);
}

QWidget *SyncPwm::getWidget(){
    return spwmWindow;
}

SyncPwm::~SyncPwm(){}
