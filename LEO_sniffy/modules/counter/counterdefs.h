#ifndef COUNTERDEFS_H
#define COUNTERDEFS_H

#include <QtGlobal>

/* COMMON defines */
#define HISTORY_SIZE        1000
#define LITERAL_FREQUENCY   "FREQUENCY"
#define LITERAL_PERIOD      "PERIOD"
#define LITERAL_NO_DATA     " "

/* HIGH FREQUENCY defines */
#define AVERAGE_RANGE       200

#define LABELNUM_CHAN       0
#define LABELNUM_PINS       1
#define LABELNUM_QUAN       2
#define LABELNUM_FLAG       3
#define LABELNUM_INDIC      4

/* LOW FREQUENCY defines */
#define SAMPLE_COUNT        100
#define CHANNEL_1           0
#define CHANNEL_2           1

/* RATIO defines */
#define LITERAL_RATIO       "RATIO"

#define REFERENCE_COUNT_DEFAULT 2000000
#define REFERENCE_COUNT_MAX     4096000000

/* INTERVALS defines */
#define INTERVAL_TIMEOUT_MAX     3600
#define INTERVAL_TIMEOUT_DEFAULT 5
#define LITERAL_INTERVAL         "TIME INTERVAL"


#endif // COUNTERDEFS_H
