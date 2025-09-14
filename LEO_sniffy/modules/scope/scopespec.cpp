#include "scopespec.h"
#include "modules/specutils.h"

#include <QDebug>
#include <QDataStream>

ScopeSpec::ScopeSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void ScopeSpec::parseSpecification(QByteArray spec){

    QDataStream stream(spec);

    int tmpVref;
    int tmpVrefInt;

    stream >> resources >> maxSamplingRate12B >> maxSamplingRate8BInterleaved ;
    stream >> memorySize>> maxADCChannels >> tmpVref >> tmpVrefInt;

    Vref = tmpVref;
    VrefInt = tmpVrefInt;

    SpecParsing::readPins4(stream, maxADCChannels, [&](int i, const QString &pin){ channelPins[i] = pin; });
}

