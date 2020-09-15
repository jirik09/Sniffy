#include "scopespec.h"

#include <QDebug>
#include <QDataStream>

ScopeSpec::ScopeSpec(QObject *parent)
{
    Q_UNUSED(parent);
}

void ScopeSpec::parseSpecification(QByteArray spec){

    QList<QByteArray> specParams = spec.split(':');

    if(specParams.length()>=7){
        QDataStream streamSampling12B(specParams[0]);
        streamSampling12B>>maxSamplingRate12B;

        QDataStream streamSampling8BInt(specParams[1]);
        streamSampling8BInt>>maxSamplingRate8BInterleaved;

        QDataStream streamBufferSize(specParams[2]);
        streamBufferSize>>memorySize;

        QDataStream streamADCChannels(specParams[3]);
        streamADCChannels>>maxADCChannels;

        QDataStream streamVref(specParams[4]);
        int tmpVref;
        streamVref>>tmpVref;
        Vref = tmpVref;

        QDataStream streamVrefInt(specParams[5]);
        int tmpVrefInt;
        streamVrefInt>>tmpVrefInt;
        VrefInt = tmpVrefInt;

        for (int i = 0;i<maxADCChannels;i++){
            channelPins[i]=specParams[6+i];
        }
        isSpecificationLoaded = true;
    }
}

