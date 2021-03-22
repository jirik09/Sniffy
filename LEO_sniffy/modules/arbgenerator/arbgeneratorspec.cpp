#include "arbgeneratorspec.h"

ArbGeneratorSpec::ArbGeneratorSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void ArbGeneratorSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> resources >> maxSamplingRate >>periphClockFrequency >> generatorBufferSize;
    stream >> DACResolution >> maxDACChannels;

    int tmp;
    stream >> tmp;
    rangeMin = (qreal)(tmp)/1000;
    stream >> tmp;
    rangeMax = (qreal)(tmp)/1000;
    stream >> tmp;
    Vcc = (qreal)(tmp)/1000;
    stream >> tmp;
    Vref = (qreal)(tmp)/1000;

    char chars[4] = "";
    for(int i = 0; i < maxDACChannels; i++){
        stream.readRawData(chars, 4);
        channelPins[i] = QString::fromUtf8(chars,4);
        channelPins[i].remove('_');
    }
}
