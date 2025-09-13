#include "pwmgeneratorconfig.h"
#include <QDataStream>
#include <QIODevice>

PWMGeneratorConfig::PWMGeneratorConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void PWMGeneratorConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    //TODO parse data from stream
    //example:
    //stream >> realSamplingRate;
    //stream >> ADCresolution;

    isConfigurationLoaded = true;

}

QByteArray PWMGeneratorConfig::serialize()
{
    QByteArray data; // stack allocated
    QDataStream stream(&data, QIODevice::WriteOnly);

    //TODO put data into stream
    //example:
    //stream << realSamplingRate;
    //stream << ADCresolution;

    return data; // NRVO / move
}
