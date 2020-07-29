#ifndef COUNTERDEFS_H
#define COUNTERDEFS_H

enum class CounterQuantity
{
  FREQUENCY = 0,
  PERIOD,
};

enum class ErrorType
{
  SIMPLE = 0,
  AVERAGE,
};

static const int AVERAGE_RANGE = 200;

#endif // COUNTERDEFS_H
