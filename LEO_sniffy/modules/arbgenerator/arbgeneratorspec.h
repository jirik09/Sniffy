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
    void parsePWMSpecification(QByteArray spec);

    int maxSamplingRate;
    int periphClockFrequency;
    int generatorBufferSize;
    int DACResolution;
    int maxDACChannels;
    int maxPWMChannels;
    int periphPWMClockFrequency;

    QString channelPins[MAX_ARB_CHANNELS_NUM];
    QString channelPWMPins[MAX_ARB_CHANNELS_NUM];

    qreal rangeMin;
    qreal rangeMax;
    qreal AVddReal;

};

#endif // ARBGENERATORSPEC_H
