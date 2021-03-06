#include "scopespec.h"

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

    char chars[4] = "";
    for(int i = 0; i < maxADCChannels; i++){
        stream.readRawData(chars, 4);
        channelPins[i] = QString(chars).left(4);
        channelPins[i].remove('_');
    }
}

