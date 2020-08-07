#ifndef COUNTERCONFIG_H
#define COUNTERCONFIG_H

#include <QObject>
#include "counterdefs.h"

class CounterConfig : public QObject
{
    Q_OBJECT
public:
    explicit CounterConfig(QObject *parent = nullptr);

    CounterMode mode = CounterMode::HIGH_FREQUENCY;

    /* Direct Frequency measurement (High Frequency) */
    CounterQuantity quantity = CounterQuantity::FREQUENCY;
    QuantitySwitched quantState = QuantitySwitched::NO;
    GateTime gateTimeSel = GateTime::GATE_TIME_100M;
    ErrorType error = ErrorType::SIMPLE;
    HoldOnState hold = HoldOnState::OFF;

    uint gateTime = 100; // ms

    /* Reciprocal Frequency measurement (Low Frequency) */


signals:

};

#endif // COUNTERCONFIG_H
