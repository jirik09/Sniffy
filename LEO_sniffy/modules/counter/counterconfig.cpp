#include "counterconfig.h"

CounterConfig::CounterConfig(QObject *parent) : QObject(parent)
{
    lfCh1States.lfQuantity = LFStates::Quantity::FREQUENCY;
    lfCh1States.multiplier = LFStates::Multiplier::MULT_1X;
    lfCh1States.dutyCycle = LFStates::DutyCycle::DISABLED;
    lfCh1States.sampleCountBackup = 1;
    lfCh2States.lfQuantity = LFStates::Quantity::FREQUENCY;
    lfCh2States.multiplier = LFStates::Multiplier::MULT_1X;
    lfCh2States.dutyCycle = LFStates::DutyCycle::DISABLED;
    lfCh2States.sampleCountBackup = 1;
}
