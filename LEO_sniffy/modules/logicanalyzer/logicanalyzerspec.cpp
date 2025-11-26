#include "logicanalyzerspec.h"
#include <QDataStream>
#include "modules/specutils.h"

LogicAnalyzerSpec::LogicAnalyzerSpec()
{
}

void LogicAnalyzerSpec::parseSpecification(QByteArray spec)
{
    QDataStream stream(spec);    

    stream >> resources;
    stream >> postTriggerClock;
    stream >> timeBaseClock;
    stream >> maxSamplingFreq;
    stream >> bufferLength;
    stream >> numChannels;

    SpecParsing::readPins4(stream, numChannels, [&](int i, const QString &pin){ channelNames.append(pin); });

    quint32 gpioA = 0, gpioB = 0, gpioC = 0, gpioD = 0;
    stream >> gpioA >> gpioB >> gpioC >> gpioD;
    setGpioMasks(gpioA, gpioB, gpioC, gpioD);
}
