#ifndef PATTERNGENERATORDEFS_H
#define PATTERNGENERATORDEFS_H

// Note: no Qt-specific includes required here

#define PATTERNS_USE_QCUSTOM_PLOT 0

#define PATTERNS_NUM 15
#define PATT_MAX_CHANNELS_NUM 8
#define PATT_MIN_GEN_FREQ (float)0.01

#define PATT_RANGE_CHAN1_LOG1 24
#define PATT_RANGE_CHAN1_LOG0 22

#define PATT_DEFAULT_GEN_FREQ 1000
#define PATT_DEFAULT_DATA_LENGTH 8

enum class ModeRW
{
    RAED = 0,
    WRITE
};

// New pattern-related enums and defaults
enum class LineCodeType
{
    NRZ_L = 0,
    RZ,
    Manchester,
    Miller,
    BiPhaseMark,
    BiPhaseSpace
};

// Reasonable defaults for new patterns
#define PATT_DEFAULT_PRBS_ORDER 7
#define PATT_DEFAULT_PWM_DUTY 50      // percent
#define PATT_DEFAULT_JOHNSON_PHASES 4 // channels used (max 8)
#define PATT_DEFAULT_PDM_LEVEL 50     // target level percent
#define PATT_DEFAULT_PARBUS_WIDTH 8   // bits

#endif // PATTERNGENERATORDEFS_H
