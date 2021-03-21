#include "syncpwmconfig.h"

SyncPwmConfig::SyncPwmConfig(QObject *parent)
{
    Q_UNUSED(parent);

    SyncPwmConfig::layout = SyncPwmLayout::VERTICAL;
}

void SyncPwmConfig::parse(QByteArray config){

}

QByteArray SyncPwmConfig::serialize(){

}
