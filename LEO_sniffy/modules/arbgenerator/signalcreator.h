#ifndef SIGNALCREATOR_H
#define SIGNALCREATOR_H

#include <QObject>

#include "arbgeneratordefs.h"

class SignalCreator : public QObject
{
    Q_OBJECT
public:
    explicit SignalCreator( QObject *parent = nullptr);

    static QList<int> createSignal(SignalShape shape, int numSamples, qreal amplitude, qreal offset, qreal duty, qreal phase, int resolution);

signals:

};

#endif // SIGNALCREATOR_H
