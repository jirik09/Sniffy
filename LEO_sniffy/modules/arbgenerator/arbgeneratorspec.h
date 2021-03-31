#ifndef ARBGENERATORSPEC_H
#define ARBGENERATORSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"
#include "arbgeneratordefs.h"

class ArbGeneratorSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit ArbGeneratorSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int maxSamplingRate;
    int periphClockFrequency;
    int generatorBufferSize;
    int DACResolution;
    int maxDACChannels;

    QString channelPins[MAX_ARB_CHANNELS_NUM];

    qreal rangeMin;
    qreal rangeMax;
    qreal Vcc;
    qreal Vref;

};

#endif // ARBGENERATORSPEC_H
