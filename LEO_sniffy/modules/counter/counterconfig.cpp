#include "counterconfig.h"

CounterConfig::CounterConfig(QObject *parent) : QObject(parent)
{
    mode = CounterMode::HIGH_FREQUENCY;

    hfState.quantity = HFState::Quantity::FREQUENCY;
    hfState.quantState = HFState::QuantitySwitched::NO;
    hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    hfState.error = HFState::ErrorType::SIMPLE;
    hfState.hold = HFState::HoldOnState::OFF;

    lfState.activeChan = LFState::ActiveChan::CHAN1;
    lfState.chan1.quantity = LFState::Channel::Quantity::FREQUENCY;
    lfState.chan1.multiplier = LFState::Channel::Multiplier::MULT_1X;
    lfState.chan1.dutyCycle = LFState::Channel::DutyCycle::DISABLED;
    lfState.chan1.sampleCountBackup = 1;
    lfState.chan2.quantity = LFState::Channel::Quantity::FREQUENCY;
    lfState.chan2.multiplier = LFState::Channel::Multiplier::MULT_1X;
    lfState.chan2.dutyCycle = LFState::Channel::DutyCycle::DISABLED;
    lfState.chan2.sampleCountBackup = 1;
}
