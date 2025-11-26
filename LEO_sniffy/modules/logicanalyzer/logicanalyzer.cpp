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
    connect(window, &LogicAnalyzerWindow::streamModeChanged, this, &LogicAnalyzer::updateStreamMode);

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
                // Each byte encodes 8 channel bits (bit0=CH1 .. bit7=CH8)
                int sampleCount = rawData.size();
                for (int i = 0; i < sampleCount; ++i)
                {
                    uchar b = static_cast<uchar>(rawData[i]);
                    for (int ch = 0; ch < 8; ++ch)
                    {
                        channelData[ch].append((b >> ch) & 0x1);
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
                        channelData[ch].append((val >> (6 + ch)) & 1);
                    }
                }
            }
            window->showData(channelData, config->sampleRate);
            // Reset per-segment state
            pendingDataLengthBytes = 0;
            currentSegmentBitPacked = false;
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

void LogicAnalyzer::updateStreamMode(bool continuous)
{
    continuousMode = continuous;
    if (!comm)
        return;
    QByteArray modeToken = continuous ? Commands::LOG_ANLYS_STREAM_CONT : Commands::LOG_ANLYS_STREAM_BATCH;
    comm->write(Commands::LOG_ANLYS, Commands::LOG_ANLYS_MODE, modeToken);
}

void LogicAnalyzer::startCapture()
{
    isRunning = true;
    lastSequence = 0;
    if (window)
        window->resetStream();
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
