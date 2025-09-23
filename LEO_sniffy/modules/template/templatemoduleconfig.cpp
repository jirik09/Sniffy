#include "templatemoduleconfig.h"
#include <QDataStream>
#include <QIODevice>

TemplateModuleConfig::TemplateModuleConfig(QObject *parent)
{
    Q_UNUSED(parent);
}

void TemplateModuleConfig::parse(QByteArray config)
{
    QDataStream stream(config);

    //TODO parse data from stream
    //example:
    //stream >> realSamplingRate;
    //stream >> ADCresolution;

    isConfigurationLoaded = true;

}

QByteArray TemplateModuleConfig::serialize()
{
    QByteArray data; // stack allocation
    QDataStream stream(&data, QIODevice::WriteOnly);

    //TODO put data into stream
    //example:
    //stream << realSamplingRate;
    //stream << ADCresolution;

    return data;
}
