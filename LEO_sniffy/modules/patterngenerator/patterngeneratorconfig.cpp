#include "patterngeneratorconfig.h"

PatternGeneratorConfig::PatternGeneratorConfig(QObject *parent)
{
    Q_UNUSED(parent);

    const int firstFivePatts = 5;
    for(int i = 0; i < firstFivePatts; i++){
        freq[i] = PATT_DEFAULT_GEN_FREQ;
        dataLen[i] = PATT_DEFAULT_DATA_LENGTH;
    }
}

void PatternGeneratorConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    stream >> pattIndex;

    for(int i = 0; i < PATTERNS_NUM; i++){
        stream >> freq[i];
        stream >> dataLen[i];
    }

    isConfigurationLoaded = true;
}

QByteArray PatternGeneratorConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    stream << pattIndex;

    for(int i = 0; i < PATTERNS_NUM; i++){
        stream << freq[i];
        stream << dataLen[i];
    }

    return *data;
}
