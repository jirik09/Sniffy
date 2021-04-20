#ifndef VOLTAGESOURCESPEC_H
#define VOLTAGESOURCESPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "../abstractspecification.h"
#include "voltagesourcedefs.h"

class VoltageSourceSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit VoltageSourceSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int DACResolution;
    int maxDACChannels;

    QString channelPins[MAX_VOLTAGE_SOURCE_CHANNELS];

    qreal rangeMin;
    qreal rangeMax;
    qreal AVddReal;
    qreal VddDefault;
};

#endif // VOLTAGESOURCESPEC_H
