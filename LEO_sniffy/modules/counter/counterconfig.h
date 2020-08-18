#ifndef COUNTERCONFIG_H
#define COUNTERCONFIG_H

#include <QObject>

enum class CounterMode
{
    HIGH_FREQUENCY = 0,
    LOW_FREQUENCY = 1,
    REFERENCE = 2,
    INTERVAL = 3,
};

struct HFState
{
    enum class Quantity { FREQUENCY = 0, PERIOD = 1 } quantity;
    enum class QuantitySwitched { NO = 0, YES = 1 } quantState;
    enum class ErrorType { SIMPLE = 0, AVERAGE = 1 } error;
    enum class HoldOnState { OFF = 0, ON = 1 } hold;
    enum class GateTime { GATE_TIME_100M = 100, GATE_TIME_500M = 500,
                          GATE_TIME_1S   = 1000, GATE_TIME_5S  = 5000,
                          GATE_TIME_10S  = 10000 } gateTime;

    int gateTimeIndexBackup;
};



struct LFState
{
    enum class ActiveChan { CHAN1 = 0, CHAN2 = 1 } activeChan;

    struct Channel
    {
        enum class Quantity { FREQUENCY = 0, PERIOD = 1 } quantity;
        enum class DutyCycle { DISABLED = 0, ENABLED = 1 } dutyCycle;
        enum class Multiplier { MULT_1X = 0, MULT_2X = 1,
                                MULT_4X = 2, MULT_8X = 3 } multiplier;

        int sampleCountBackup;
    } chan1, chan2;
};

struct RefState
{

};

class CounterConfig : public QObject
{
    Q_OBJECT
public:
    explicit CounterConfig(QObject *parent = nullptr);

    CounterMode mode;
    HFState hfState;
    LFState lfState;

signals:

};



#endif // COUNTERCONFIG_H
