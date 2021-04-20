#include "voltagesourcespec.h"

VoltageSourceSpec::VoltageSourceSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void VoltageSourceSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> resources;
    stream >> DACResolution >> maxDACChannels;

    int tmp;
    stream >> tmp;
    rangeMin = (qreal)(tmp)/1000;
    stream >> tmp;
    rangeMax = (qreal)(tmp)/1000;
    stream >> tmp;
    AVddReal = (qreal)(tmp)/1000;

    char chars[4] = "";
    for(int i = 0; i < maxDACChannels; i++){
        stream.readRawData(chars, 4);
        channelPins[i] = QString::fromUtf8(chars,4);
        channelPins[i].remove('_');
    }
}
