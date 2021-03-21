#include "arbgeneratorconfig.h"

ArbGeneratorConfig::ArbGeneratorConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void ArbGeneratorConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    //TODO parse data from stream
    //example:
    //stream >> realSamplingRate;
    //stream >> ADCresolution;

    isConfigurationLoaded = true;

}

QByteArray ArbGeneratorConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    //TODO put data into stream
    //example:
    //stream << realSamplingRate;
    //stream << ADCresolution;

    return *data;
}
