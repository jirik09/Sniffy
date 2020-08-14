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

    /* High Frequency counter */
    StatesHF statesHF;
    StatesHF::Quantity quantity = StatesHF::Quantity::FREQUENCY;
    StatesHF::QuantitySwitched quantState = StatesHF::QuantitySwitched::NO;
    StatesHF::GateTime gateTime = StatesHF::GateTime::GATE_TIME_100M;
    StatesHF::ErrorType error = StatesHF::ErrorType::SIMPLE;
    StatesHF::HoldOnState hold = StatesHF::HoldOnState::OFF;

    /* Low Frequency counter */


signals:

};

#endif // COUNTERCONFIG_H
