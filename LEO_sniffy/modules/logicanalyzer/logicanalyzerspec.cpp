#include "logicanalyzerspec.h"
#include <QDataStream>
#include <QDebug>
#include "modules/specutils.h"

LogicAnalyzerSpec::LogicAnalyzerSpec()
{
    qDebug() << "LogicAnalyzerSpec::LogicAnalyzerSpec()";
    postTriggerClock = 0;
    timeBaseClock = 0;
    maxSamplingFreq = 0;
    bufferLength = 0;
    numChannels = 0;
    gpioMaskA = 0;
    gpioMaskB = 0;
    gpioMaskC = 0;
    gpioMaskD = 0;
}

void LogicAnalyzerSpec::parseSpecification(QByteArray data)
{
    qDebug() << "LogicAnalyzerSpec::parseSpecification() - Data size:" << data.size();
    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    // Minimum payload: 5 uint32 + 4 masks = 9 * 4 = 36 bytes (without channel names)
    if (data.size() < 20)
    { // at least first five uint32s
        qDebug() << "LogicAnalyzerSpec::parseSpecification() - ERROR: Payload too small";
        isSpecificationLoaded = false;
        return;
    }

    stream >> postTriggerClock;
    stream >> timeBaseClock;
    stream >> maxSamplingFreq;
    stream >> bufferLength;
    stream >> numChannels;

    qDebug() << "LogicAnalyzerSpec::parseSpecification() - Parsed header: postTrigClock=" << postTriggerClock
             << " timeBaseClock=" << timeBaseClock
             << " maxSamplingFreq=" << maxSamplingFreq
             << " bufferLength=" << bufferLength
             << " numChannels=" << numChannels;

    // Sanity checks to prevent freezes/crashes on corrupt data
    if (numChannels == 0 || numChannels > 64)
    {
        qDebug() << "LogicAnalyzerSpec::parseSpecification() - WARNING: numChannels invalid ->" << numChannels;
        numChannels = 0; // neutralize
        isSpecificationLoaded = false;
        return;
    }

    const int expectedMinSize = 20 + (int)numChannels * 4 + 16; // header + names + 4 masks
    if (data.size() < expectedMinSize)
    {
        qDebug() << "LogicAnalyzerSpec::parseSpecification() - WARNING: Payload smaller than expected (" << data.size() << "/" << expectedMinSize << ")";
        // We'll still try but guard reads.
    }

    channelNames.clear();
    channelNames.reserve((int)numChannels);
    for (quint32 i = 0; i < numChannels; ++i)
    {
        QString pin;
        if (!SpecParsing::readOnePin4(stream, pin))
        {
            qDebug() << "LogicAnalyzerSpec::parseSpecification() - WARNING: truncated channel name at index" << i;
            break;
        }
        channelNames.append(pin);
    }

    quint32 a = 0, b = 0, c = 0, d = 0;
    if (!stream.atEnd())
        stream >> a;
    else
        a = 0;
    if (!stream.atEnd())
        stream >> b;
    else
        b = 0;
    if (!stream.atEnd())
        stream >> c;
    else
        c = 0;
    if (!stream.atEnd())
        stream >> d;
    else
        d = 0;

    setGpioMasks(a, b, c, d);

    isSpecificationLoaded = true;
    qDebug() << "LogicAnalyzerSpec::parseSpecification() - Completed. Channels=" << channelNames
             << " GPIO masks A:" << QString::number(getGpioMaskA(), 16)
             << " B:" << QString::number(getGpioMaskB(), 16)
             << " C:" << QString::number(getGpioMaskC(), 16)
             << " D:" << QString::number(getGpioMaskD(), 16);
}
