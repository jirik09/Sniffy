#include <QTimer>
#include <QDebug>
#include "logicanalyzer.h"
#include "communication/commands.h"
#include "communication/comms.h"

LogicAnalyzer::LogicAnalyzer(QObject *parent) : AbstractModule(parent)
{
    qDebug() << "LogicAnalyzer::LogicAnalyzer() - Start";
    moduleName = "Logic Analyzer";
    moduleCommandPrefix = Commands::LOG_ANLYS;
    // moduleIconURI = ":/icons/logicanalyzer.png"; // Placeholder
    
    config = new LogicAnalyzerConfig(this);
    qDebug() << "LogicAnalyzer::LogicAnalyzer() - Config created";
    spec = new LogicAnalyzerSpec();
    qDebug() << "LogicAnalyzer::LogicAnalyzer() - Spec created";
    // Register spec with base so generic helpers (GPIO masks, resource gating) work
    moduleSpecification = spec;
    window = new LogicAnalyzerWindow(config);
    qDebug() << "LogicAnalyzer::LogicAnalyzer() - Window created";
    
    // Connect config signals to module slots if needed
    // connect(config, &LogicAnalyzerConfig::someSignal, this, &LogicAnalyzer::someSlot);

    // Temporary: Emit description immediately to ensure it shows up during development
    // Ideally, this should happen after receiving config from MCU
    /*
    QTimer::singleShot(1000, this, [this](){
        qDebug() << "LogicAnalyzer::LogicAnalyzer() - Emitting description";
        QList<QString> labels;
        QList<QString> values;
        labels << "Channels" << "Max Freq";
        values << "8" << "Unknown"; 
        emit moduleDescription(moduleName, labels, values);
        // showModuleControl(); // Don't show control automatically, let user open it
    });
    */
    qDebug() << "LogicAnalyzer::LogicAnalyzer() - End";
}

LogicAnalyzer::~LogicAnalyzer()
{
    qDebug() << "LogicAnalyzer::~LogicAnalyzer()";
    delete window;
    delete spec;
    // config is child of this, so it will be deleted automatically
}

QWidget* LogicAnalyzer::getWidget()
{
    return window;
}

void LogicAnalyzer::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);
    // DeviceMediator strips the module prefix (LAN_), so we receive CFG_<payload>
    if (dataHeader == Commands::CONFIG) {
        qDebug() << "LogicAnalyzer::parseData() - Received CONFIG";
        parseConfiguration(data.mid(4));
        return;
    }

    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    while (!stream.atEnd()) {
        char header[5];
        if (stream.readRawData(header, 4) < 4) break;
        header[4] = '\0';
        QByteArray cmd(header);

        if (cmd == Commands::LOG_ANLYS_TRIGGER_POINTER) {
            quint32 ptr;
            stream >> ptr;
            // TODO: Use trigger pointer
        } else if (cmd == Commands::LOG_ANLYS_DATA_LENGTH) {
            quint32 len;
            stream >> len;
            // TODO: Use data length
        } else if (cmd == Commands::LOG_ANLYS_DATA) {
            QByteArray rawData = stream.device()->readAll();
            
            QVector<QVector<int>> channelData(8);
            const quint16 *samples = reinterpret_cast<const quint16*>(rawData.constData());
            int sampleCount = rawData.size() / 2;
            
            for (int i = 0; i < sampleCount; ++i) {
                quint16 val = samples[i];
                for (int ch = 0; ch < 8; ++ch) {
                    channelData[ch].append((val >> (6 + ch)) & 1);
                }
            }
            
            // TODO: Get actual sample rate
            window->showData(channelData, 10000); 
            break;
        }
    }
}

void LogicAnalyzer::writeConfiguration()
{
    // TODO: Send configuration to MCU
}

void LogicAnalyzer::parseConfiguration(QByteArray config)
{
    qDebug() << "LogicAnalyzer::parseConfiguration() - Payload size:" << config.size();
    spec->parseSpecification(config);
    if (!spec->isLoaded()) {
        qDebug() << "LogicAnalyzer::parseConfiguration() - Specification invalid or not loaded";
        return;
    }
    
    // Update UI with spec
    QList<QString> labels;
    QList<QString> values;
    labels << "Channels" << "Max Freq";
    values << QString::number(spec->numChannels) << QString::number(spec->maxSamplingFreq);
    
    emit moduleDescription(moduleName, labels, values);
    showModuleControl();
}

QByteArray LogicAnalyzer::getConfiguration()
{
    return QByteArray();
}

void LogicAnalyzer::startModule()
{
    if (comm) {
        qDebug() << "LogicAnalyzer::startModule() - Requesting configuration (LAN_:CFG?)";
        comm->write(Commands::LOG_ANLYS, Commands::CONFIG_REQUEST);
    }
    comm->write(Commands::LOG_ANLYS, Commands::INIT);
    comm->write(Commands::LOG_ANLYS, Commands::START);
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
