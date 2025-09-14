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
}
