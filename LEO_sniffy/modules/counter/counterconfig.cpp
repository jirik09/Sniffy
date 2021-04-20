#include "counterconfig.h"

CounterConfig::CounterConfig(QObject *parent)
{
    Q_UNUSED(parent);

    mode = CounterMode::HIGH_FREQUENCY;
    modePrevIndex = CounterMode::HIGH_FREQUENCY;    

    hfState.quantity = HFState::Quantity::FREQUENCY;
    hfState.quantState = HFState::QuantitySwitched::NO;
    hfState.gateTime = HFState::GateTime::GATE_TIME_100M;
    hfState.error = HFState::ErrorType::SIMPLE;
    hfState.hold = HFState::HoldOnState::OFF;
    hfState.gateTimeIndexBackup = 0;

    lfState.activeChan = LFState::ActiveChan::CHAN1;
    lfState.chan1.quantity = LFState::Channel::Quantity::FREQUENCY;
    lfState.chan1.multiplier = LFState::Channel::Multiplier::MULT_1X;
    lfState.chan1.dutyCycle = LFState::Channel::DutyCycle::DISABLED;
    lfState.chan1.sampleCount = 1;
    lfState.chan2.quantity = LFState::Channel::Quantity::FREQUENCY;
    lfState.chan2.multiplier = LFState::Channel::Multiplier::MULT_1X;
    lfState.chan2.dutyCycle = LFState::Channel::DutyCycle::DISABLED;
    lfState.chan2.sampleCount = 1;

    ratState.sampleCount = REFERENCE_COUNT_DEFAULT;    

    intState.seqAB = IntState::Sequence::AB;
    intState.eventA = IntState::Event::RISING;
    intState.eventB = IntState::Event::RISING;
    intState.timeout = INTERVAL_TIMEOUT_DEFAULT;
    intState.running = false;
}

void CounterConfig::parse(QByteArray config)
{
    QDataStream stream(config);
    /* General settings */
    stream >> mode >> modePrevIndex;

    /* High Freq settings */
    stream >> hfState.quantity >> hfState.quantState >> hfState.gateTime ;
    stream >> hfState.error >> hfState.hold >> hfState.gateTimeIndexBackup;

    /* Low Freq settings */
    stream >> lfState.activeChan;
    stream >> lfState.chan1.quantity >> lfState.chan1.multiplier;
    stream >> lfState.chan1.dutyCycle >> lfState.chan1.sampleCount;
    stream >> lfState.chan2.quantity >> lfState.chan2.multiplier;
    stream >> lfState.chan2.dutyCycle >> lfState.chan2.sampleCount;

    /* Freq Ration settings */
    stream >> ratState.sampleCount;

    /* Intervals meas. settings */
    stream >> intState.seqAB >> intState.eventA >> intState.eventB >> intState.timeout;
}

QByteArray CounterConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    /* General settings */
    stream << mode << modePrevIndex;

    /* High Freq settings */
    stream << hfState.quantity << hfState.quantState << hfState.gateTime ;
    stream << hfState.error << hfState.hold << hfState.gateTimeIndexBackup;

    /* Low Freq settings */
    stream << lfState.activeChan;
    stream << lfState.chan1.quantity << lfState.chan1.multiplier;
    stream << lfState.chan1.dutyCycle << lfState.chan1.sampleCount;
    stream << lfState.chan2.quantity << lfState.chan2.multiplier;
    stream << lfState.chan2.dutyCycle << lfState.chan2.sampleCount;

    /* Freq Ration settings */
    stream << ratState.sampleCount;

    /* Intervals meas. settings */
    stream << intState.seqAB << intState.eventA << intState.eventB << intState.timeout;

    return *data;
}
