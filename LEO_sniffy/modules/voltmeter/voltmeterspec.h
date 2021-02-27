#ifndef VOLTMETERSPEC_H
#define VOLTMETERSPEC_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include "voltmeterdefs.h"
#include "../abstractspecification.h"

class VoltmeterSpec : public AbstractSpecification
{
    Q_OBJECT
public:
    explicit VoltmeterSpec(QObject *parent = nullptr);
    void parseSpecification(QByteArray spec);

    int maxSamplingRate12B;
    int maxSamplingRate8BInterleaved;
    int memorySize;
    int maxADCChannels;
    QString channelPins[MAX_VOLTMETER_CHANNELS];
    qreal Vref;
    qreal VrefCalibration;

};

#endif // VOLTMETERSPEC_H
