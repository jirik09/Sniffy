#include "syncpwmspec.h"
#include "modules/specutils.h"

SyncPwmSpec::SyncPwmSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void SyncPwmSpec::parseSpecification(QByteArray spec){
    QDataStream stream(spec);
    stream >> resources;
    stream >> chans_depend >> drive_chx >> drive_chy >> driven_chx >> driven_chy;
    stream >> max_freq >> chan_num;

    SpecParsing::readPins4(stream, pinsList.size(), [&](int i, const QString &pin){ pinsList[i] = pin; });

    pins.chan1 = pinsList.at(0);
    pins.chan2 = pinsList.at(1);
    pins.chan3 = pinsList.at(2);
    pins.chan4 = pinsList.at(3);
}
