#include "voltmeterconfig.h"

VoltmeterConfig::VoltmeterConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void VoltmeterConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    //TODO parse data from stream
    //example:
    //stream >> realSamplingRate;
    //stream >> ADCresolution;

    isConfigurationLoaded = true;

}

QByteArray VoltmeterConfig::serialize()
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
