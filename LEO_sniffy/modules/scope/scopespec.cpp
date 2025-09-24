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

    // New: read 4x GPIO masks (A,B,C,D)
    quint32 gpioA = 0, gpioB = 0, gpioC = 0, gpioD = 0;
    stream >> gpioA >> gpioB >> gpioC >> gpioD;
    setGpioMasks(gpioA, gpioB, gpioC, gpioD);
}

