#ifndef SIGNALCREATOR_H
#define SIGNALCREATOR_H

#include <QObject>
#include <limits.h>
#include <float.h>
#include <QtMath>
#include <QtDebug>
#include "arbgeneratordefs.h"


class SignalCreator : public QObject
{
    Q_OBJECT
public:
    explicit SignalCreator( QObject *parent = nullptr);

    static QList<qreal> createSignal(SignalShape shape, int numSamples, qreal amplitude, qreal offset, qreal duty, qreal phase, qreal rangeMin, qreal rangeMax);
    static int calculateSignalLength(MemoryLength memSet, int customLength, int generatorBufferSize, qreal signalFreq, int maxSamplingRate, int periphClockFrequency);
signals:

};

#endif // SIGNALCREATOR_H
