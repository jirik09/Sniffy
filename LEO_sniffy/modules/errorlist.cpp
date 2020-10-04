#include "errorlist.h"

ErrorList::ErrorList(QObject *parent) : QObject(parent)
{
}

QString ErrorList::GetErrMessage(uint value)
{
    switch (value) {
    case 0:
        return "NO_ERROR_CODE";
    case 1:
        return "SYSTEM_INVALID_FEATURE";
    case 2:
        return "COMMS_INVALID_FEATURE";
    case 9:
        return "UNSUPORTED_FUNCTION_ERR";
    case 50:
        return "SCOPE_INVALID_FEATURE";
    case 54:
        return "SCOPE_INVALID_FEATURE_PARAM";
    case 55:
        return "SCOPE_UNSUPPORTED_RESOLUTION";
    case 56:
        return "SCOPE_INVALID_TRIGGER_CHANNEL";
    case 57:
        return "SCOPE_INVALID_SAMPLING_FREQ";
    case 58:
        return "BUFFER_SIZE_ERR";
    case 61:
        return "SCOPE_INVALID_ADC_CHANNEL";
    case 100:
        return "GEN_INVALID_FEATURE";
    case 101:
        return "GEN_INVALID_STATE";
    case 102:
        return "GEN_OUT_OF_MEMORY";
    case 103:
        return "GEN_BUFFER_SIZE_ERR";
    case 104:
        return "GEN_MISSING_DATA";
    case 105:
        return "GEN_TO_HIGH_FREQ";
    case 107:
        return "GEN_FREQ_IS_INACCURATE";
    case 108:
        return "GEN_FREQ_MISMATCH";
    case 109:
        return "GEN_INVALID_DATA";
    case 110:
        return "GEN_DAC_SET_ERROR";
    case 130:
        return "CNT_FEATURE_MODE_MISMATCH";
    case 150:
        return "COUNTER_INVALID_FEATURE_PARAM";
    case 151:
        return "SYNC_PWM_INVALID_FEATURE";
    case 152:
        return "LOG_ANLYS_INVALID_FEATURE";
    case 255:
        return "UNKNOW_ERROR";
    default:
        return "Undefined Error message:"+QString::number(value);
    }
}
