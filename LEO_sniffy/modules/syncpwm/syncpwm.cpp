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

void SyncPwm::startModule(){}

void SyncPwm::stopModule(){
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
            //int chanIndex = data.remove(0, 4).toUInt();
        }
    }
}

void SyncPwm::writeConfiguration(){
    spwmWindow->restoreGUIAfterStartup();

    write(cmd->SPWM_COMMAND, cmd->SPWM_INIT);

    if(config->common.stepMode){
        switchStepModeCallback(1);
    }else {
        switchStepModeCallback(0);
    }

    for(int ch = 0; ch < CHANNELS_NUM; ch++){
        setFreq(config->chan[ch].freq, ch);
        setDuty(config->chan[ch].duty, ch);
        setInvert(ch);
        setOnOff(!config->chan[ch].enabled, ch);
    }

    spwmWindow->repaint();
}

void SyncPwm::parseConfiguration(QByteArray config){
    Q_UNUSED(config);
    this->config->parse(config);
}

QByteArray SyncPwm::getConfiguration(){
    return config->serialize();
}

/****************************** Functions ******************************/

void SyncPwm::stepGenEndNotif(){
    spwmWindow->setStartTxt();
    spwmWindow->uncheckStartButton();
    config->state = State::STOPPED;
}

void SyncPwm::setFreq(float val, int chanIndex){
    config->chan[chanIndex].freq = val;
    quint32 freq = (quint32)(val * FREQ_PRECISION);
    QByteArray data = intToSend(freq)+":"+intToSend(FREQ_PRECISION)+":"+intToSend(chanIndex);
    comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_FREQ_CONFIG+":"+data+";");
}

void SyncPwm::setDuty(float val, int chanIndex){
    config->chan[chanIndex].duty = val;
    quint32 dutyCycle = (quint32)(val * DUTY_PRECISION);
    quint32 phase = (quint32)(config->chan[chanIndex].phase * PHASE_PRECISION);
    setDutyPhase(dutyCycle, phase, chanIndex);
}

void SyncPwm::setPhase(float val, int chanIndex){
    config->chan[chanIndex].phase = val;
    quint32 phase = (quint32)(val * PHASE_PRECISION);
    quint32 dutyCycle = (quint32)(config->chan[chanIndex].duty * DUTY_PRECISION);
    setDutyPhase(dutyCycle, phase, chanIndex);
}

void SyncPwm::setOnOff(int index, int chanIndex){
    bool enable = (index == 0) ? true : false;
    config->chan[chanIndex].enabled = enable;
    write(cmd->SPWM_CHANNEL_STATE, (chanIndex)<<8 | enable);
    spwmWindow->enableChannel(enable, chanIndex);
}

void SyncPwm::setInvert(int chanIndex){
    config->chan[chanIndex].inverted = spwmWindow->settings->inverCh[chanIndex]->isChecked(0);
    write(cmd->SPWM_CHANNEL_INVERT, (chanIndex)<<8 | config->chan[chanIndex].inverted);
}

void SyncPwm::equiModeConfig(){
    skipRepaintLock = true;
    equiModeSetFreq(config->chan[0].freq);
    equiModeSetPhase(config->chan[0].phase);
    equiModeSetDuty(config->chan[0].duty);
    skipRepaintLock = false;
}

void SyncPwm::equiModeSetFreq(float val){
    for(int ch = 1; ch < CHANNELS_NUM; ch++){
        setFreq(val, ch);
        spwmWindow->setFreqDial(val, ch);
    }

    if(!skipRepaintLock)
        spwmWindow->repaint();
}

void SyncPwm::equiModeSetPhase(float val){
    for(int ch = 1; ch < CHANNELS_NUM; ch++){
        setPhase(val*(ch+1), ch);
        spwmWindow->setPhaseDial(val*(ch+1), ch);
    }

    if(!skipRepaintLock)
        spwmWindow->repaint();
}

void SyncPwm::equiModeSetDuty(float val){
    for(int ch = 1; ch < CHANNELS_NUM; ch++){
        setDuty(val, ch);
        spwmWindow->setDutyDial(val, ch);
    }

    if(!skipRepaintLock)
        spwmWindow->repaint();
}

void SyncPwm::setDutyPhase(quint32 dutyCycle, quint32 phase, quint32 chanIndex){
    comm->write(cmd->SYNC_PWM_GEN+":"+cmd->SPWM_DUTYPHASE_CONFIG
                +":"+intToSend(dutyCycle)
                +":"+intToSend(DUTY_PRECISION)
                +":"+intToSend(phase)
                +":"+intToSend(PHASE_PRECISION)
                +":"+intToSend(chanIndex)+";");
}

/******************************** Callbacks *********************************/

void SyncPwm::buttonStartCallback(int index){
    Q_UNUSED(index);
    if(config->state == State::RUNNING){
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
    bool enable = (index == 0) ? true : false;
    if(enable) {
        config->common.stepMode = false;
        write(cmd->SPWM_STEP_MODE, cmd->SPWM_STEP_DISABLE);
    }else {
        config->common.stepMode = true;
        write(cmd->SPWM_STEP_MODE, cmd->SPWM_STEP_ENABLE);
    }
    stepGenEndNotif();
    spwmWindow->repaint();
}

void SyncPwm::buttonEquidistantModeCallback(int index){
    Q_UNUSED(index);
    if(config->common.equiMode){
        config->common.equiMode = false;
    }else {
        config->common.equiMode = true;
        equiModeConfig();
        spwmWindow->repaint();
    }
}

void SyncPwm::switchOnOffCallback(int index, int chanIndex){
    setOnOff(index, chanIndex);
    spwmWindow->repaint();
}

void SyncPwm::buttonInvertCallback(int index, int chanIndex){
    Q_UNUSED(index);
    setInvert(chanIndex);
    spwmWindow->repaint();
}

void SyncPwm::dialFreqCallback(float val, int chanIndex){    
    setFreq(val, chanIndex);
    if(config->common.equiMode && chanIndex == 0)
        equiModeSetFreq(val);
    else
        spwmWindow->repaint();
}

void SyncPwm::dialPhaseCallback(float val, int chanIndex){
    setPhase(val, chanIndex);
    if(config->common.equiMode && chanIndex == 0)
        equiModeSetPhase(val);
    else
        spwmWindow->repaint();
}

void SyncPwm::dialDutyCallback(float val, int chanIndex){
    setDuty(val, chanIndex);
    if(config->common.equiMode && chanIndex == 0)
        equiModeSetDuty(val);
    else
        spwmWindow->repaint();
}

/******************************** Common Fun *********************************/

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
