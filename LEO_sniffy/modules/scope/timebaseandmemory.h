#ifndef TIMEBASEANDMEMORY_H
#define TIMEBASEANDMEMORY_H

#include <QObject>
#include <QtMath>

#include "scopeconfig.h"
#include "scopespec.h"
#include "scopedefs.h"

class TimeBaseAndMemory : public QObject
{
    Q_OBJECT
public:
    explicit TimeBaseAndMemory(ScopeConfig *config, QObject *parent = nullptr);
    void handleMemoryPolicy();
    void setMaxParams(int memorySize, int numChannels);
    void setTimeBase(qreal secPerDiv);
    void overWriteSamplingFreq(int freq);
    void overWriteMemoryLength(int samples);
    void setMemoryPolicy(MemoryPolicy policy);
    void setNumOfChannels(int chanNum);
    void setDataResolution(int bits);
    qreal getZoom() const;

private:
    ScopeConfig *config;
    int maxNumChannels;
    int maxMemorySize;
    qreal totalnumOfDiv;

signals:
    void updateSamplingFrequency(int freq);
    void updateMemorySamplesLength(int memLength);
    void updateNumChannels(int numChannels);
    void updateDataResolution(int bits);
};

#endif // TIMEBASEANDMEMORY_H
