#include "patterngeneratorconfig.h"

PatternGeneratorConfig::PatternGeneratorConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void PatternGeneratorConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    stream >> prevIndex;

    isConfigurationLoaded = true;

}

QByteArray PatternGeneratorConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    stream << prevIndex;

    return *data;
}
