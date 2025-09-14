#include "voltagesourcespec.h"
#include "modules/common/specutils.h"

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
    stream >> tmp;
    VddDefault = (qreal)(tmp)/1000;

    SpecParsing::readPins4(stream, maxDACChannels, [&](int i, const QString &pin){ channelPins[i] = pin; });
}
