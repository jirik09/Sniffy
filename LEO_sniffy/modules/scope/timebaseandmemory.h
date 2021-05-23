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

    qreal getNumOfDiv() const;
    qreal getZoom() const;

private:
    ScopeConfig *config;
    int maxNumChannels;
    int maxMemorySize;

    qreal totalnumOfDiv;
   // int samplesPerDiv = DEFAULT_SAMPL_PER_DIV;
  // int samplingFrequency;

 //   int dataResolution;
 //   int numChannels;
 //   MemoryPolicy memoryPolicy;



  //  void validate();

signals:
    void updateSamplingFrequency(int freq);
    void updateMemorySamplesLength(int memLength);
    void updateNumChannels(int numChannels);

};

#endif // TIMEBASEANDMEMORY_H
