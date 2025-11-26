#ifndef LOGICANALYSER_SPEC_H
#define LOGICANALYSER_SPEC_H

#include "../abstractspecification.h"

class LogicAnalyzerSpec : public AbstractSpecification
{
public:
    explicit LogicAnalyzerSpec();
    void parseSpecification(QByteArray spec) override;

    int postTriggerClock;
    int timeBaseClock;
    int maxSamplingFreq;
    int bufferLength;
    int numChannels;
    QList<QString> channelNames;
};

#endif // LOGICANALYSER_SPEC_H
