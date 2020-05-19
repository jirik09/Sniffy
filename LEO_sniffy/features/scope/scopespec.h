#ifndef SCOPESPEC_H
#define SCOPESPEC_H

#include <QObject>
#include "scopedefs.h"

class ScopeSpec : public QObject
{
    Q_OBJECT
public:
    explicit ScopeSpec(QObject *parent = nullptr);

    int getMaxSamplingRate() const;
    void setMaxSamplingRate(int value);

    int getMemorySize() const;
    void setMemorySize(int value);

private:
    int maxSamplingRate;
    int memorySize;
    QString channelPins[MAX_SCOPE_CHANNELS];
    int Vref;


signals:

};

#endif // SCOPESPEC_H
