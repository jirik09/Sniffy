#include "scopespec.h"

ScopeSpec::ScopeSpec(QObject *parent) : QObject(parent)
{

}

int ScopeSpec::getMaxSamplingRate() const
{
    return maxSamplingRate;
}

void ScopeSpec::setMaxSamplingRate(int value)
{
    maxSamplingRate = value;
}

int ScopeSpec::getMemorySize() const
{
    return memorySize;
}

void ScopeSpec::setMemorySize(int value)
{
    memorySize = value;
}
