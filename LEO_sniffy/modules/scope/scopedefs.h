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
};

#define MAX_SCOPE_CHANNELS 4
#define DEFAULT_MEM_SAMPLES_LENGTH 1200
#define DEFAULT_CHART_DIV 10
#define DEFAULT_SAMPL_PER_DIV 100

#endif // SCOPEDEFS_H
