#include "patterngeneratorspec.h"

PatternGeneratorSpec::PatternGeneratorSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void PatternGeneratorSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);

    stream >> resources;

    char chars[4] = "";
    for(int i = 0; i < chanPins->size(); i++){
        stream.readRawData(chars, 4);
        chanPins[i] = QString(chars);
        chanPins[i].remove('_');
    }
}
