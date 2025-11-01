#include "patterngeneratorspec.h"
#include "modules/specutils.h"

PatternGeneratorSpec::PatternGeneratorSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void PatternGeneratorSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> resources >> maxSamplingRate;

    SpecParsing::readPins4(stream, PATT_MAX_CHANNELS_NUM, [&](int i, const QString &pin){ chanPins[i] = pin; });

    // New: read 4x GPIO masks (A,B,C,D)
    quint32 gpioA = 0, gpioB = 0, gpioC = 0, gpioD = 0;
    stream >> gpioA >> gpioB >> gpioC >> gpioD;
    setGpioMasks(gpioA, gpioB, gpioC, gpioD);
}
