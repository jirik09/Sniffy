#ifndef LOGICANALYSER_SPEC_H
#define LOGICANALYSER_SPEC_H

#include "../abstractspecification.h"

class LogicAnalyzerSpec : public AbstractSpecification
{
public:
    LogicAnalyzerSpec();
    void parseSpecification(QByteArray data) override;

    quint32 postTriggerClock;
    quint32 timeBaseClock;
    quint32 maxSamplingFreq;
    quint32 bufferLength;
    quint32 numChannels;
    QList<QString> channelNames;
};

#endif // LOGICANALYSER_SPEC_H
