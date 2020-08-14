#ifndef COUNTERDEFS_H
#define COUNTERDEFS_H

#include <QtGlobal>

enum class CounterMode
{
    HIGH_FREQUENCY = 0,
    LOW_FREQUENCY = 1,
    REFERENCE = 2,
    INTERVAL = 3,
};

struct StatesHF
{
    enum class Quantity { FREQUENCY = 0, PERIOD = 1 };
    enum class QuantitySwitched { NO = 0, YES = 1 };
    enum class ErrorType { SIMPLE = 0, AVERAGE = 1 };
    enum class HoldOnState { OFF = 0, ON = 1 };
    enum class GateTime { GATE_TIME_100M = 100, GATE_TIME_500M = 500,
                          GATE_TIME_1S   = 1000, GATE_TIME_5S   = 5000,
                          GATE_TIME_10S  = 10000 };

    int gateTimeBackupIndex;
    int avgBackupSampleCount;
};

struct StatesLF
{

};

struct StatesRef
{

};

/* HIGH FREQUENCY defines */

#define AVERAGE_RANGE       200

#define LABELNUM_CHAN       0
#define LABELNUM_PINS       1
#define LABELNUM_QUAN       2
#define LABELNUM_HOLD       3
#define LABELNUM_FLAG       4

/* HIGH FREQUENCY defines */

#define SAMPLE_COUNT        100

/* COMMON defines */

#define LITERAL_FREQUENCY   "FREQUENCY"
#define LITERAL_PERIOD      "PERIOD"

#endif // COUNTERDEFS_H
