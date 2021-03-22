#ifndef ARBGENERATORDEFS_H
#define ARBGENERATORDEFS_H

#define MAX_ARB_CHANNELS_NUM 4

enum class SignalShape
{
    SINE = 0,
    SAW = 1,
    RECT = 2,
    ARB = 3
};

enum class MemoryLength
{
    BEST_FIT = 0,
    LONG = 1,
    CUSTOM = 2,
};

#endif // ARBGENERATORDEFS_H
