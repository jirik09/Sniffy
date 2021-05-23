#include "timebaseandmemory.h"

TimeBaseAndMemory::TimeBaseAndMemory(ScopeConfig *config,QObject *parent) :  QObject(parent), config(config)
{
    totalnumOfDiv = qreal(DEFAULT_MEM_SAMPLES_LENGTH)/DEFAULT_SAMPL_PER_DIV;
  //  samplesPerDiv = DEFAULT_SAMPL_PER_DIV;
}

void TimeBaseAndMemory::setMaxParams(int memorySize, int numChannels)
{
    maxMemorySize = memorySize;
    maxNumChannels = numChannels;
}

void TimeBaseAndMemory::setTimeBase(qreal secPerDiv)
{
    int tmpFreq;
    if(config->memPolicy == MemoryPolicy::LONG){
        tmpFreq = round(float(DEFAULT_SAMPL_PER_DIV)/(secPerDiv)+0.49);
    }else{
        tmpFreq = round(float(config->dataLength)/(totalnumOfDiv*secPerDiv)+0.49);
    }
    if(tmpFreq != config->requestedSamplingRate){
        config->requestedSamplingRate = tmpFreq;
        emit updateSamplingFrequency(config->requestedSamplingRate);
        config->timeBase = secPerDiv;
    }
}

void TimeBaseAndMemory::setSamplingFreq(int freq)
{
    config->requestedSamplingRate = freq;
    emit updateSamplingFrequency(config->requestedSamplingRate);
}

void TimeBaseAndMemory::setMemoryLength(int samples)
{
    int tmpMaxSamples = maxMemorySize / config->numberOfChannels / (config->ADCresolution>8?2:1);
    config->dataLength = fmin(samples,tmpMaxSamples);
    totalnumOfDiv = qreal(config->dataLength)/DEFAULT_SAMPL_PER_DIV;
    emit updateMemorySamplesLength(config->dataLength);

   /* config->memPolicy = MemoryPolicy::NORMAL;
    int tmpMaxSamples = maxMemorySize / config->numberOfChannels / (config->ADCresolution>8?2:1);
    config->dataLength = fmin(samples,tmpMaxSamples);
    samplesPerDiv = config->dataLength / DEFAULT_CHART_DIV;
    totalnumOfDiv = DEFAULT_CHART_DIV;
    setTimeBase((float)config->dataLength/config->requestedSamplingRate/DEFAULT_CHART_DIV);
    emit updateMemorySamplesLength(config->dataLength);*/
}

void TimeBaseAndMemory::setMemoryLength(MemoryPolicy policy)
{
    if(policy!= config->memPolicy){
        config->memPolicy= policy;
        handleMemoryPolicy();
    }
}

void TimeBaseAndMemory::setNumOfChannels(int chanNum)
{
    if(config->numberOfChannels != chanNum && chanNum<=maxNumChannels){
        if(config->numberOfChannels>=chanNum){ //to set less channels
            config->numberOfChannels = chanNum;
            emit updateNumChannels(config->numberOfChannels);
            handleMemoryPolicy();
        }else{  //add more channels
            config->numberOfChannels = chanNum;
            handleMemoryPolicy();
            emit updateNumChannels(config->numberOfChannels);
        }
    }
}

void TimeBaseAndMemory::setDataResolution(int bits)
{
    //TODO
    config->ADCresolution = bits;
}

void TimeBaseAndMemory::handleMemoryPolicy(){
    int tmpMemLen;
    switch (config->memPolicy) {
    case MemoryPolicy::NORMAL:
        tmpMemLen = DEFAULT_MEM_SAMPLES_LENGTH;
        break;
    case MemoryPolicy::LONG:
    case MemoryPolicy::ZOOM:
        tmpMemLen = maxMemorySize / config->numberOfChannels / (config->ADCresolution>8?2:1);
        break;
    }
  //  totalnumOfDiv = qreal(tmpMemLen)/DEFAULT_SAMPL_PER_DIV;
  //  samplesPerDiv = DEFAULT_SAMPL_PER_DIV;

    if(tmpMemLen != config->dataLength){
        config->dataLength = tmpMemLen;
        emit updateMemorySamplesLength(config->dataLength);
    }
    setTimeBase(config->timeBase);
}

qreal TimeBaseAndMemory::getNumOfDiv() const
{
  //  return totalnumOfDiv;
    return 0;
}

qreal TimeBaseAndMemory::getDefaultZoom() const
{
    return qreal(DEFAULT_MEM_SAMPLES_LENGTH)/DEFAULT_SAMPL_PER_DIV/DEFAULT_CHART_DIV;
}
