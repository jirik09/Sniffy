#ifndef SCOPESPEC_H
#define SCOPESPEC_H

#include <QObject>
#include "scopedefs.h"
#include "../abstractspecification.h"

class ScopeSpec : public AbstractSpecification
{
    Q_OBJECT

public:
    explicit ScopeSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int maxSamplingRate12B;
    int maxSamplingRate8BInterleaved;
    int memorySize;
    int maxADCChannels;
    QString channelPins[MAX_SCOPE_CHANNELS];
    qreal Vref;
    qreal VrefInt;
};

#endif // SCOPESPEC_H
