#include "patterngeneratorconfig.h"

PatternGeneratorConfig::PatternGeneratorConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void PatternGeneratorConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    stream >> pattIndex;

    for(int i = 0; i < PATTERNS_NUM; i++)
        stream >> freq[i];

    isConfigurationLoaded = true;
}

QByteArray PatternGeneratorConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    stream << pattIndex;

    for(int i = 0; i < PATTERNS_NUM; i++)
        stream << freq[i];

    return *data;
}
