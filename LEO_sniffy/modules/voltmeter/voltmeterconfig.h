#ifndef VOLTMETERCONFIG_H
#define VOLTMETERCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"
#include "voltmeterdefs.h"

class VoltmeterConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit VoltmeterConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();

    int realSamplingRate;
    int ADCresolution;
    int dataLength;

    int rangeMin; //mV
    int rangeMax; //mv
    int numberOfChannels = 1;

    QString datalogFileName;
};

#endif // VOLTMETERCONFIG_H
