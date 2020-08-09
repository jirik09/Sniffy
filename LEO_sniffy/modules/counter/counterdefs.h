#ifndef COUNTERDEFS_H
#define COUNTERDEFS_H

#include <QtGlobal>

enum class CounterMode
{
    HIGH_FREQUENCY = 0,
    LOW_FREQUENCY,
    REFERENCE,
    INTERVAL,
};

enum class CounterQuantity
{
    FREQUENCY = 0,
    PERIOD,
};

enum class QuantitySwitched
{
    YES = 0,
    NO,
};

enum class ErrorType
{
    SIMPLE = 0,
    AVERAGE,
};

enum class HoldOnState
{
    ON = 0,
    OFF,
};

enum class GateTime
{
    GATE_TIME_100M = 0,
    GATE_TIME_500M,
    GATE_TIME_1S,
    GATE_TIME_5S,
    GATE_TIME_10S,
};

#define AVERAGE_RANGE       200

#define LABELNUM_CHAN       0
#define LABELNUM_PINS       1
#define LABELNUM_QUAN       2
#define LABELNUM_HOLD       3
#define LABELNUM_FLAG       4

#define LITERAL_FREQUENCY   "FREQUENCY"
#define LITERAL_PERIOD      "PERIOD"

#endif // COUNTERDEFS_H
