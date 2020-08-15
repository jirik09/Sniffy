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
    HFStates hfStates;
    HFStates::Quantity hfQuantity = HFStates::Quantity::FREQUENCY;
    HFStates::QuantitySwitched quantState = HFStates::QuantitySwitched::NO;
    HFStates::GateTime gateTime = HFStates::GateTime::GATE_TIME_100M;
    HFStates::ErrorType error = HFStates::ErrorType::SIMPLE;
    HFStates::HoldOnState hold = HFStates::HoldOnState::OFF;

    /* Low Frequency counter */
    LFActiveChan activeChan = LFActiveChan::CHAN1;
    LFStates lfCh1States;
    LFStates lfCh2States;

signals:

};

#endif // COUNTERCONFIG_H
