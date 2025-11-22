#include "logicanalyzerspec.h"
#include <QDataStream>
#include <QDebug>

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

    // Skip "LOGA" or "CFG_" header if present (handled by caller usually, but let's be safe)
    // Actually caller removes header.
    
    // MCU sends:
    // Comms_SendUint32(LOG_ANLYS_POSTTRIG_PERIPH_CLOCK);
    // Comms_SendUint32(LOG_ANLYS_TIMEBASE_PERIPH_CLOCK);
    // Comms_SendUint32(LOG_ANLYS_SAMPLING_FREQ);
    // Comms_SendUint32(LOG_ANLYS_BUFFER_LENGTH);
    // Comms_SendUint32(LOG_ANLYS_CHANNELS_NUM);
    // Channels names...
    // GPIO masks...

    // Minimum payload: 5 uint32 + 4 masks = 9 * 4 = 36 bytes (without channel names)
    if (data.size() < 20) { // at least first five uint32s
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
    if (numChannels == 0 || numChannels > 64) {
        qDebug() << "LogicAnalyzerSpec::parseSpecification() - WARNING: numChannels invalid ->" << numChannels;
        numChannels = 0; // neutralize
        isSpecificationLoaded = false;
        return;
    }

    const int expectedMinSize = 20 + (int)numChannels * 4 + 16; // header + names + 4 masks
    if (data.size() < expectedMinSize) {
        qDebug() << "LogicAnalyzerSpec::parseSpecification() - WARNING: Payload smaller than expected (" << data.size() << "/" << expectedMinSize << ")";
        // We'll still try but guard reads.
    }

    channelNames.clear();
    for(quint32 i=0; i<numChannels; ++i) {
        char buffer[5];
        if (stream.readRawData(buffer, 4) < 4) {
            qDebug() << "LogicAnalyzerSpec::parseSpecification() - WARNING: truncated channel name at index" << i;
            break;
        }
        buffer[4] = '\0';
        channelNames.append(QString(buffer));
    }

    if (!stream.atEnd()) stream >> gpioMaskA; else gpioMaskA = 0;
    if (!stream.atEnd()) stream >> gpioMaskB; else gpioMaskB = 0;
    if (!stream.atEnd()) stream >> gpioMaskC; else gpioMaskC = 0;
    if (!stream.atEnd()) stream >> gpioMaskD; else gpioMaskD = 0;

    isSpecificationLoaded = true;
    qDebug() << "LogicAnalyzerSpec::parseSpecification() - Completed. Channels=" << channelNames
             << " GPIO masks A:" << QString::number(gpioMaskA,16)
             << " B:" << QString::number(gpioMaskB,16)
             << " C:" << QString::number(gpioMaskC,16)
             << " D:" << QString::number(gpioMaskD,16);
}
