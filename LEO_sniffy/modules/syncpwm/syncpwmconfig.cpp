#include "syncpwmconfig.h"

SyncPwmConfig::SyncPwmConfig(QObject *parent)
{
    Q_UNUSED(parent);

    SyncPwmConfig::layout = Layout::VERTICAL;
    SyncPwmConfig::state = State::STOPPED;
    SyncPwmConfig::common.stepMode = false;
    SyncPwmConfig::common.equiMode = false;

    int phase = 0;
    for(int i = 0; i < CHANNELS_NUM; i++){
        SyncPwmConfig::chan[i].enabled = true;
        SyncPwmConfig::chan[i].inverted = false;
        SyncPwmConfig::chan[i].frequency = DEFAULT_FREQUENCY;
        SyncPwmConfig::chan[i].dutyCycle = DEFAULT_DC;
        SyncPwmConfig::chan[i].phase = phase;
        phase += PI_HALF;
    }
}

void SyncPwmConfig::parse(QByteArray config)
{
    QDataStream stream(config);
    /* General settings */
    SyncPwmConfig::state = State::STOPPED;
    stream >> layout >> state >> common.stepMode >> common.equiMode;

    /* Channels settings */
    for(int i = 0; i < CHANNELS_NUM; i++){
        stream >> SyncPwmConfig::chan[i].enabled;
        stream >> SyncPwmConfig::chan[i].inverted;
        stream >> SyncPwmConfig::chan[i].frequency;
        stream >> SyncPwmConfig::chan[i].dutyCycle;
        stream >> SyncPwmConfig::chan[i].phase;
    }
}

QByteArray SyncPwmConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    SyncPwmConfig::state = State::STOPPED;
    stream << layout << state << common.stepMode << common.equiMode;

    for(int i = 0; i < CHANNELS_NUM; i++){
        stream << SyncPwmConfig::chan[i].enabled;
        stream << SyncPwmConfig::chan[i].inverted;
        stream << SyncPwmConfig::chan[i].frequency;
        stream << SyncPwmConfig::chan[i].dutyCycle;
        stream << SyncPwmConfig::chan[i].phase;
    }

    return *data;
}
