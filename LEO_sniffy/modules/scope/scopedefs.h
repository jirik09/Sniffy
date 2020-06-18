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

#define MAX_SCOPE_CHANNELS 4

#endif // SCOPEDEFS_H
