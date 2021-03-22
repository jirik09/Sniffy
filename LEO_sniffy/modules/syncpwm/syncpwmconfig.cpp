#include "syncpwmconfig.h"

SyncPwmConfig::SyncPwmConfig(QObject *parent)
{
    Q_UNUSED(parent);

    SyncPwmConfig::layout = Layout::VERTICAL;
    SyncPwmConfig::state = State::STOPPED;
    SyncPwmConfig::common.stepMode = false;
    SyncPwmConfig::common.equiMode = false;

    int phase = 0;
    for(int i = 1; i < CHANNELS_NUM; i++){
        SyncPwmConfig::chan[i].frequency = 1;
        SyncPwmConfig::chan[i].dutyCycle = 0;
        SyncPwmConfig::chan[i].phase = phase;
        phase += PI_HALF;
    }
}

void SyncPwmConfig::parse(QByteArray config){

}

QByteArray SyncPwmConfig::serialize(){

}
