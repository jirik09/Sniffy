#include "syncpwmconfig.h"

SyncPwmConfig::SyncPwmConfig(QObject *parent)
{
    Q_UNUSED(parent);

    SyncPwmConfig::layout = SyncPwmLayout::VERTICAL;
    SyncPwmConfig::state = SyncPwmState::STOPPED;
    SyncPwmConfig::equiMode = SyncPwmModeEqui::EQUI_DISABLED;
}

void SyncPwmConfig::parse(QByteArray config){

}

QByteArray SyncPwmConfig::serialize(){

}
