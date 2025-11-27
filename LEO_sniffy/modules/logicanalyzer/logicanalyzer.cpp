#include <QTimer>
#include <QDebug>
#include <QtEndian>
#include "../labelformator.h"
#include "logicanalyzer.h"
#include "communication/commands.h"
#include "communication/comms.h"

LogicAnalyzer::LogicAnalyzer(QObject *parent) : AbstractModule(parent)
{
    moduleName = "Logic Analyzer";
    moduleCommandPrefix = Commands::LOG_ANLYS;
    // moduleIconURI = ":/icons/logicanalyzer.png"; // Placeholder

    config = new LogicAnalyzerConfig(this);
    spec = new LogicAnalyzerSpec();
    // Register spec with base so generic helpers (GPIO masks, resource gating) work
    moduleSpecification = spec;
    window = new LogicAnalyzerWindow(config);

    // Connect config signals to module slots
    connect(window, &LogicAnalyzerWindow::startCapture, this, &LogicAnalyzer::startCapture);
    connect(window, &LogicAnalyzerWindow::stopCapture, this, &LogicAnalyzer::stopCapture);
    connect(window, &LogicAnalyzerWindow::sampleRateChanged, this, &LogicAnalyzer::updateSampleRate);
    connect(window, &LogicAnalyzerWindow::triggerChannelChanged, this, &LogicAnalyzer::updateTriggerChannel);
    connect(window, &LogicAnalyzerWindow::triggerEdgeChanged, this, &LogicAnalyzer::updateTriggerEdge);
    connect(window, &LogicAnalyzerWindow::triggerModeChanged, this, &LogicAnalyzer::updateTriggerMode);

    qDebug() << "LogicAnalyzer::LogicAnalyzer() - End";
}

LogicAnalyzer::~LogicAnalyzer()
{
    qDebug() << "LogicAnalyzer::~LogicAnalyzer()";
    delete window;
    delete spec;
    // config is child of this, so it will be deleted automatically
}

QWidget *LogicAnalyzer::getWidget()
{
    return window;
}

void LogicAnalyzer::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);
    // DeviceMediator strips the module prefix (LAN_), so we receive CFG_<payload>
    if (dataHeader == Commands::CONFIG)
    {
        qDebug() << "LogicAnalyzer::parseData() - Received CONFIG";
        parseConfiguration(data.mid(4));
        return;
    }

    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    while (!stream.atEnd())
    {
        char header[5];
        if (stream.readRawData(header, 4) < 4)
            break;
        header[4] = '\0';
        QByteArray cmd(header);

        if (cmd == Commands::LOG_ANLYS_SEQ)
        {
            quint32 seq;
            stream >> seq;
            if (lastSequence != 0 && seq != lastSequence + 1)
            {
                qDebug() << "[LogicAnalyzer] Sequence gap detected. Expected" << (lastSequence + 1) << "got" << seq;
            }
            // If this is the first sequence of a new batch (seq == 1), clear the chart
            // Note: We already clear in startCapture/REQ_NEXT, but this is a safety net
            // if (seq == 1) {
            //    window->resetStream();
            // }
            lastSequence = seq;
        }
        else if (cmd == Commands::LOG_ANLYS_PACKING)
        {
            quint32 fmt;
            stream >> fmt;
            currentSegmentBitPacked = (fmt == 1);
            qDebug() << "[LogicAnalyzer] Packing format:" << (currentSegmentBitPacked ? "packed8" : "raw16");
        }
        else if (cmd == Commands::LOG_ANLYS_COMPLETE)
        {
            qDebug() << "[LogicAnalyzer] Capture complete received.";
            // No payload expected; could trigger UI finalize if needed
        }
        else if (cmd == Commands::LOG_ANLYS_TRIGGER_POINTER)
        {
            quint32 ptr;
            stream >> ptr;
            // TODO: Use trigger pointer
        }
        else if (cmd == Commands::LOG_ANLYS_DATA_LENGTH)
        {
            quint32 len;
            stream >> len;
            pendingDataLengthBytes = len;
        }
        else if (cmd == Commands::LOG_ANLYS_DATA)
        {
            QByteArray rawData = stream.device()->readAll();
            QVector<QVector<int>> channelData(8);

            if (currentSegmentBitPacked)
            {
                // Each byte encodes 8 channel bits. Device packs GPIO channels
                // such that the first channel corresponds to the MSB of the byte.
                // Map bits accordingly so UI channel 0 receives the correct bit.
                int sampleCount = rawData.size();
                for (int i = 0; i < sampleCount; ++i)
                {
                    uchar b = static_cast<uchar>(rawData[i]);
                    for (int ch = 0; ch < 8; ++ch)
                    {
                        channelData[ch].append((b >> (7 - ch)) & 0x1);
                    }
                }
            }
            else
            {
                // Raw 16-bit IDR samples
                int sampleCount = rawData.size() / 2;
                const uchar *rawBytes = reinterpret_cast<const uchar *>(rawData.constData());
                for (int i = 0; i < sampleCount; ++i)
                {
                    quint16 val = qFromLittleEndian<quint16>(rawBytes + (i * 2));
                    for (int ch = 0; ch < 8; ++ch)
                    {
                        /* Device encodes GPIO bits into IDR such that channel 1
                           maps to bit 13 and channel 8 to bit 6 (reverse order).
                           Reverse the channel index so UI receives correct ordering. */
                        channelData[ch].append((val >> (13 - ch)) & 1);
                    }
                }
            }
            window->showData(channelData, config->sampleRate);
            // Reset per-segment state
            pendingDataLengthBytes = 0;
            currentSegmentBitPacked = false;

            // Handle Continuous vs Single (Batch) mode loop
            if (isRunning) {
                if (continuousMode) {
                    // Request next batch; FW will restart only when in WAIT_FOR_RESTART
                    window->resetStream();
                    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_REQ_NEXT);
                } else {
                    // Single shot finished: stop requesting and revert UI
                    isRunning = false;
                    if (window) {
                        window->singleSamplingDone();
                    }
                }
            }
            break;
        }
    }
}

void LogicAnalyzer::writeConfiguration()
{
    // Scope based example:

    // scpWindow->restoreGUIAfterStartup();
    // scpWindow->setNumChannels(specification->maxADCChannels);
    // scpWindow->setRealSamplingRateAndLlength(config->realSamplingRate,config->dataLength);

    // comm->write(moduleCommandPrefix+":"+cmd->SCOPE_ADC_CHANNEL_DEAFULT+";");

    // timeAndMemoryHandle->setMaxParams(specification->memorySize,specification->maxADCChannels);

    // setResolution(config->ADCresolution);
    // setDataLength(config->dataLength);
    // setSamplingFrequency(config->requestedSamplingRate);
    // updateTriggerLevel(config->triggerLevelPercent);
    // updatePretrigger(config->pretriggerPercent);
    // updateTriggerEdge(config->triggerEdge);
    // setNumberOfChannels(config->numberOfChannels);
    // updateTriggerChannel(config->triggerChannelIndex);
    // if(config->triggerMode==ScopeTriggerMode::TRIG_STOP)
    //     config->triggerMode=ScopeTriggerMode::TRIG_SINGLE;
    // setTriggerMode(config->triggerMode);
}

void LogicAnalyzer::parseConfiguration(QByteArray config)
{
    qDebug() << "LogicAnalyzer::parseConfiguration() - Payload size:" << config.size();
    spec->parseSpecification(config);
    // Pass specification to window for potential channel label usage
    window->setSpecification(spec->channelNames);

    // Set the sample number to the maximum buffer length reported by the device
    if (spec->bufferLength > 0) {
        comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_SAMPLES_NUM, spec->bufferLength);
    }

    // Ensure control is shown before emitting description
    showModuleControl();
    buildModuleDescription();
}

QByteArray LogicAnalyzer::getConfiguration()
{
    return QByteArray();
}

void LogicAnalyzer::startModule()
{
    // CONFIG? already sent in AbstractModule::setComms
    comm->write(Commands::LOG_ANLYS, Commands::INIT);
    // comm->write(Commands::LOG_ANLYS, Commands::START); // Don't start immediately, let user start
}

void LogicAnalyzer::stopModule()
{
    comm->write(Commands::LOG_ANLYS, Commands::STOP);
    comm->write(Commands::LOG_ANLYS, Commands::DEINIT);
}

void LogicAnalyzer::updateSampleRate(int rate)
{
    config->sampleRate = rate;
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_SAMPLING_FREQ, rate);

    // Also update post-trigger time as it depends on sample rate
    double postTriggerSec = (double)spec->bufferLength / config->sampleRate;

    uint32_t words[2];
    memcpy(words, &postTriggerSec, sizeof(double));

    QByteArray payload;
    payload.append(Commands::LOG_ANLYS);
    payload.append(':');
    payload.append(Commands::LOG_ANLYS_POSTTRIG);

    char highBytes[4];
    highBytes[0] = (words[1] >> 0) & 0xFF;
    highBytes[1] = (words[1] >> 8) & 0xFF;
    highBytes[2] = (words[1] >> 16) & 0xFF;
    highBytes[3] = (words[1] >> 24) & 0xFF;
    payload.append(highBytes, 4);

    char lowBytes[4];
    lowBytes[0] = (words[0] >> 0) & 0xFF;
    lowBytes[1] = (words[0] >> 8) & 0xFF;
    lowBytes[2] = (words[0] >> 16) & 0xFF;
    lowBytes[3] = (words[0] >> 24) & 0xFF;
    payload.append(lowBytes, 4);
    payload.append(';');

    comm->write(payload);
}

void LogicAnalyzer::updateTriggerChannel(int channel)
{
    config->triggerChannel = channel;
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_TRIGGER_CHANNEL, channel);
}

void LogicAnalyzer::updateTriggerEdge(int edge)
{
    config->triggerEdge = edge;
    // 0: Rising, 1: Falling
    QByteArray edgeCmd = (edge == 0) ? Commands::EDGE_RISING : Commands::EDGE_FALLING;
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_TRIGGER_EVENT, edgeCmd);
}

void LogicAnalyzer::updateTriggerMode(int mode)
{
    // 0: Auto, 1: Normal, 2: Single, 3: Stop
    if (mode == 3)
    {
        stopCapture();
        return;
    }

    // Map to firmware trigger modes if needed, or just handle continuous flag
    // FW modes: LOGA_MODE_AUTO=0, LOGA_MODE_NORMAL=1, LOGA_MODE_SINGLE=2
    // These match our internal mapping perfectly!

    // Set continuous mode flag
    continuousMode = (mode == 0 || mode == 1);

    // Send mode to FW
    // Note: FW expects CMD_TRIG_MODE_... which are defined in commands.h
    // We need to send the specific command for the mode.
    QByteArray modeCmd;
    if (mode == 0) modeCmd = Commands::MODE_AUTO;
    else if (mode == 1) modeCmd = Commands::MODE_NORMAL;
    else if (mode == 2) modeCmd = Commands::MODE_SINGLE;

    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_TRIGGER_MODE, modeCmd);

    // Start capture
    startCapture();
}

void LogicAnalyzer::startCapture()
{
    isRunning = true;
    lastSequence = 0;
    if (window)
        window->resetStream();
    
    // Send configuration to ensure FW is synced
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_SAMPLES_NUM, spec->bufferLength);
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_SAMPLING_FREQ, config->sampleRate);

    // Calculate and send post-trigger time
    // Post-trigger = BufferLength / SampleRate
    // We send it as two 32-bit words (High, Low)
    double postTriggerSec = (double)spec->bufferLength / config->sampleRate;
    
    // Construct payload for POST command: LAN_:POST<High32><Low32>;
    // Important: Do NOT insert spaces between POST and the first 4 bytes.
    // The FW reads first 4 bytes with giveNextCmd() and the second 4 bytes with Comms_BufferReadUInt32().

    uint32_t words[2];
    memcpy(words, &postTriggerSec, sizeof(double));
    // words[0] is Low32, words[1] is High32 (Little Endian)
    
    QByteArray payload;
    payload.append(Commands::LOG_ANLYS);
    payload.append(':');
    payload.append(Commands::LOG_ANLYS_POSTTRIG); // POST
    
    // Append High32 (words[1])
    char highBytes[4];
    highBytes[0] = (words[1] >> 0) & 0xFF;
    highBytes[1] = (words[1] >> 8) & 0xFF;
    highBytes[2] = (words[1] >> 16) & 0xFF;
    highBytes[3] = (words[1] >> 24) & 0xFF;
    payload.append(highBytes, 4);
    
    // Append Low32 (words[0])
    char lowBytes[4];
    lowBytes[0] = (words[0] >> 0) & 0xFF;
    lowBytes[1] = (words[0] >> 8) & 0xFF;
    lowBytes[2] = (words[0] >> 16) & 0xFF;
    lowBytes[3] = (words[0] >> 24) & 0xFF;
    payload.append(lowBytes, 4);
    payload.append(';');
    
    comm->write(payload);

    // Always ensure we are in BATCH mode for PC-controlled loop
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_MODE, Commands::LOG_ANLYS_STREAM_BATCH);
    
    comm->write(Commands::LOG_ANLYS, Commands::START);
}

void LogicAnalyzer::stopCapture()
{
    isRunning = false;
    lastSequence = 0; // reset sequence tracking for next run
    if (window)
        window->resetStream();
    comm->write(Commands::LOG_ANLYS, Commands::STOP);
}

void LogicAnalyzer::buildModuleDescription()
{
    QString name = moduleName;
    QList<QString> labels, values;

    labels.append("Channels");
    values.append(QString::number(spec->numChannels));

    labels.append("Memory size");
    values.append(LabelFormator::formatOutout(spec->bufferLength, "B", 1));

    labels.append("Max sampling rate");
    values.append(LabelFormator::formatOutout(spec->maxSamplingFreq, "sps", 2));

    labels.append("Pins");
    QString pins;
    for (int i = 0; i < spec->numChannels; ++i) {
        if (i < spec->channelNames.size())
            pins += spec->channelNames[i] + ", ";
    }
    if (!pins.isEmpty())
        pins.chop(2);
    values.append(pins);

    showModuleDescription(name, labels, values);
}
