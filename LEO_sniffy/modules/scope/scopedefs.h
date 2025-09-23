#ifndef SCOPEDEFS_H
#define SCOPEDEFS_H

enum class ScopeTriggerMode
{
    TRIG_STOP = 0,
    TRIG_NORMAL,
    TRIG_AUTO,
    TRIG_AUTO_FAST,
    TRIG_SINGLE
};

enum class ScopeTriggerEdge
{
    EDGE_RISING = 0,
    EDGE_FALLING
};

enum class MemoryPolicy
{
    NORMAL = 0,
    LONG = 1,
    ZOOM = 2,
    OVERWRITE
};

#define MAX_SCOPE_CHANNELS 4
// Math trace is treated as an extra (virtual) channel appended after the real channels.
// Keep this centralized so any future change in channel layout propagates safely.
constexpr int MATH_CHANNEL_INDEX = MAX_SCOPE_CHANNELS; // e.g. 4
constexpr int TOTAL_SCOPE_TRACES = MAX_SCOPE_CHANNELS + 1; // real channels + math
#define DEFAULT_MEM_SAMPLES_LENGTH 1200
#define DEFAULT_CHART_DIV 10
#define DEFAULT_SAMPL_PER_DIV 100

#endif // SCOPEDEFS_H
