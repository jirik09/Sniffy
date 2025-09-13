#include "voltmeterconfig.h"
#include <QDataStream>
#include <QIODevice>

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
    QByteArray data; // stack allocated
    QDataStream stream(&data, QIODevice::WriteOnly);

    //TODO put data into stream
    //example:
    //stream << realSamplingRate;
    //stream << ADCresolution;

    return data;
}
