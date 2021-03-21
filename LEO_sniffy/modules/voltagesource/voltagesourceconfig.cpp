#include "voltagesourceconfig.h"

VoltageSourceConfig::VoltageSourceConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void VoltageSourceConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    //TODO parse data from stream
    //example:
    //stream >> realSamplingRate;
    //stream >> ADCresolution;

    isConfigurationLoaded = true;

}

QByteArray VoltageSourceConfig::serialize()
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
