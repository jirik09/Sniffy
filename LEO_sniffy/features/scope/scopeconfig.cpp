#include "scopeconfig.h"

ScopeConfig::ScopeConfig(QObject *parent) : QObject(parent)
{

}

int ScopeConfig::getSamplingRate() const
{
    return samplingRate;
}

void ScopeConfig::setSamplingRate(int value)
{
    samplingRate = value;
}

int ScopeConfig::getADCresolution() const
{
    return ADCresolution;
}

void ScopeConfig::setADCresolution(int value)
{
    ADCresolution = value;
}

int ScopeConfig::getDataLength() const
{
    return dataLength;
}

void ScopeConfig::setDataLength(int value)
{
    dataLength = value;
}

float ScopeConfig::getTimeBase() const
{
    return timeBase;
}

void ScopeConfig::setTimeBase(float value)
{
    timeBase = value;
}
