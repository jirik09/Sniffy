#include "counterconfig.h"

CounterConfig::CounterConfig(QObject *parent) : QObject(parent)
{
    mode = CounterMode::HIGH_FREQUENCY;
    modePrevIndex = CounterMode::HIGH_FREQUENCY;

    hfStateControl = State::RUNNING;
    hfState.quantity = HFState::Quantity::FREQUENCY;
    hfState.quantState = HFState::QuantitySwitched::NO;
    hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    hfState.error = HFState::ErrorType::SIMPLE;
    hfState.hold = HFState::HoldOnState::OFF;
    hfState.gateTimeIndexBackup = 0;

    lfStateControl = State::RUNNING;
    lfState.activeChan = LFState::ActiveChan::CHAN1;
    lfState.chan1.quantity = LFState::Channel::Quantity::FREQUENCY;
    lfState.chan1.multiplier = LFState::Channel::Multiplier::MULT_1X;
    lfState.chan1.dutyCycle = LFState::Channel::DutyCycle::DISABLED;
    lfState.chan1.sampleCount = 1;
    lfState.chan2.quantity = LFState::Channel::Quantity::FREQUENCY;
    lfState.chan2.multiplier = LFState::Channel::Multiplier::MULT_1X;
    lfState.chan2.dutyCycle = LFState::Channel::DutyCycle::DISABLED;
    lfState.chan2.sampleCount = 1;

    ratStateControl = State::RUNNING;
    ratState.sampleCount = REFERENCE_COUNT_DEFAULT;

    intState.seqAB = IntState::Sequence::AB;
    intState.eventA = IntState::Event::RISING;
    intState.eventB = IntState::Event::RISING;
    intState.timeout = INTERVAL_TIMEOUT_DEFAULT;
}
