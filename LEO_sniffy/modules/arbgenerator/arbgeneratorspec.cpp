#include "arbgeneratorspec.h"
#include "modules/specutils.h"

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
    AVddReal = (qreal)(tmp)/1000;

    SpecParsing::readPins4(stream, maxDACChannels, [&](int i, const QString &pin){ channelPins[i] = pin; });
}

void ArbGeneratorSpec::parsePWMSpecification(QByteArray spec)
{
    QDataStream stream(spec);
    resources = 0;

    stream >> resources >> DACResolution >> periphPWMClockFrequency >> maxPWMChannels >> maxSamplingRate;
    stream >> periphClockFrequency >> generatorBufferSize;

    int tmp;
    stream >> tmp;
    rangeMin = (qreal)(tmp)/1000;
    stream >> tmp;
    rangeMax = (qreal)(tmp)/1000;

    SpecParsing::readPins4(stream, maxPWMChannels, [&](int i, const QString &pin){ channelPWMPins[i] = pin; });
}
