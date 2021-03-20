#include "syncpwmspec.h"

SyncPwmSpec::SyncPwmSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void SyncPwmSpec::parseSpecification(QByteArray spec){
    QDataStream stream(spec);
    stream >> max_freq >> chan_num;

    char chars[4] = "";
    for(int i = 0; i < pinsList.size(); i++){
        stream.readRawData(chars, 4);
        pinsList[i] = QString(chars);
        pinsList[i].remove('_');
    }

    pins.chan1 = pinsList.at(0);
    pins.chan2 = pinsList.at(1);
    pins.chan3 = pinsList.at(2);
    pins.chan4 = pinsList.at(3);
}
