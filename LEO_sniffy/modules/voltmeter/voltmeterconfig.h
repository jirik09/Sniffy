#ifndef VOLTMETERCONFIG_H
#define VOLTMETERCONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "../abstractconfiguration.h"
#include "voltmeterdefs.h"

enum class VoltmeterMode
{
    NORMAL = 0,
    FAST = 1,
};

class VoltmeterConfig : public AbstractConfiguration
{
public:
    explicit VoltmeterConfig(QObject *parent = nullptr);
    void parse (QByteArray config);
    QByteArray serialize ();

    int realSamplingRate;
    int ADCresolution;

    int rangeMin; //mV
    int rangeMax; //mv
    int numberOfChannels = 1;

    QString datalogFileName;

    int targetDataLength = 200;
    VoltmeterMode mode;
};

#endif // VOLTMETERCONFIG_H
