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
        SyncPwmConfig::chan[i].dutyCycle = 25;
        SyncPwmConfig::chan[i].phase = phase;
        phase += PI_HALF;
    }
}

void SyncPwmConfig::parse(QByteArray config)
{
    QDataStream stream(config);
    /* General settings */
    stream >> layout >> state >> common.stepMode >> common.equiMode;

    /* Channels settings */
    for(int i = 1; i < CHANNELS_NUM; i++){
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

    stream << layout << state << common.stepMode << common.equiMode;

    for(int i = 1; i < CHANNELS_NUM; i++){
        stream << SyncPwmConfig::chan[i].frequency;
        stream << SyncPwmConfig::chan[i].dutyCycle;
        stream << SyncPwmConfig::chan[i].phase;
    }

    return *data;
}
