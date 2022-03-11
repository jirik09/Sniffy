#include "patterngeneratorspec.h"

PatternGeneratorSpec::PatternGeneratorSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void PatternGeneratorSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> resources >> maxSamplingRate;

    char chars[4] = "";
    for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++){
        stream.readRawData(chars, 4);
        chanPins[i] = QString(chars);
        chanPins[i].remove('_');
    }
}
