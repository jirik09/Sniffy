#include "logicanalyzerconfig.h"
#include <QDataStream>
#include <QDebug>

LogicAnalyzerConfig::LogicAnalyzerConfig(QObject *parent) : AbstractConfiguration(parent)
{
    qDebug() << "LogicAnalyzerConfig::LogicAnalyzerConfig()";
    channels.resize(8);
    for(int i=0; i<8; ++i) {
        channels[i].name = QString("CH%1").arg(i+1);
        channels[i].enabled = true;
        channels[i].color = i; // Placeholder for color index
    }
}

void LogicAnalyzerConfig::parse(QByteArray config)
{
    qDebug() << "LogicAnalyzerConfig::parse()";
    // TODO: Implement parsing of binary config from MCU
    // For now we assume defaults or parse what we can
    emit configurationChanged();
}

QByteArray LogicAnalyzerConfig::serialize()
{
    // TODO: Implement serialization if needed
    return QByteArray();
}
