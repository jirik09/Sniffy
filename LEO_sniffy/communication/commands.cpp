#include "commands.h"

Commands::Commands(QObject *parent) : QObject(parent)
{
    /* Counter lists for index-based access */
    Commands::pCOUNTER_MODE = {MODE_HIGH_FREQ, MODE_LOW_FREQ, MODE_RATIO, MODE_INTERVAL};
    Commands::pHF_GATE_TIME = {HF_TIME_100M, HF_TIME_500M, HF_TIME_1S, HF_TIME_5S, HF_TIME_10S};
    Commands::pLF_MULTIPLIER = {LF_MULTIPLIER_1X, LF_MULTIPLIER_2X, LF_MULTIPLIER_4X, LF_MULTIPLIER_8X};
    Commands::pQUANTITIY = {QUANT_FREQUENCY, QUANT_PERIOD};
}

const QByteArray Commands::IDN = "IDN_";

/* COMMUNICATION */
// General
const QByteArray Commands::IDN_REQUEST = "IDN?";
const QByteArray Commands::VERSION_REQUEST = "VER?";
const QByteArray Commands::CONFIG_REQUEST = "CFG?";
const QByteArray Commands::IS_SHIELD_CONNECTED = "SH_?";
const QByteArray Commands::RESET_DEVICE = "RES!";

const QByteArray Commands::GENERATOR = "GEN_";
const QByteArray Commands::PWM_GENERATOR = "GENP";
const QByteArray Commands::SYNC_PWM_GEN = "SPWM";
const QByteArray Commands::LOG_ANLYS = "LOGA";
const QByteArray Commands::COUNTER = "CNT_";
const QByteArray Commands::SCOPE = "OSCP";
const QByteArray Commands::SCOPE_INCOME = "OSC_";
const QByteArray Commands::COMMS = "COMS";
const QByteArray Commands::SYSTEM = "SYST";

const QByteArray Commands::STOP = "STOP";
const QByteArray Commands::START = "STRT";
const QByteArray Commands::PAUSE = "PAUS";
const QByteArray Commands::UNPAUSE = "UPAU";
const QByteArray Commands::NEXT = "NEXT";
const QByteArray Commands::CONFIG = "CFG_";

const QByteArray Commands::GET_REAL_SMP_FREQ = "FRQ?";
const QByteArray Commands::GET_SCOPE_INPUTS = "INP?";

/************************ SCOPE ***********************/
//Scope
const QByteArray Commands::CHANNELS = "CHAN";
const QByteArray Commands::DATA_LENGTH = "LENG";  //number of samples

const QByteArray Commands::DATA_LENGTH_CH1 = "LCH1";
const QByteArray Commands::DATA_LENGTH_CH2 = "LCH2";

const QByteArray Commands::SAMPLING_FREQ = "FREQ";

const QByteArray Commands::TRIGGERED = "TRIG";
const QByteArray Commands::SAMPLING = "SMPL";
const QByteArray Commands::SCOPE_OK = "S_OK";

//Scope specific commands flags
const QByteArray Commands::SCOPE_TRIG_MODE = "TRIG";
const QByteArray Commands::SCOPE_TRIG_EDGE = "EDGE";
const QByteArray Commands::SCOPE_TRIG_LEVEL = "LEVL";
const QByteArray Commands::SCOPE_TRIG_CHANNEL = "TRCH";
const QByteArray Commands::SCOPE_DATA_DEPTH = "DATA";
const QByteArray Commands::SCOPE_PRETRIGGER = "PRET";
const QByteArray Commands::SCOPE_NEXT = "NEXT";
const QByteArray Commands::SCOPE_FREQ = "OSCF";
const QByteArray Commands::SCOPE_ADC_CHANNEL = "A_CH";
const QByteArray Commands::SCOPE_ADC_CHANNEL_DEAFULT = "ADEF";
const QByteArray Commands::SCOPE_ADC_CHANNEL_VREF = "AREF";

//scope modes
const QByteArray Commands::MODE_NORMAL = "NORM";
const QByteArray Commands::MODE_AUTO = "AUTO";
const QByteArray Commands::MODE_SINGLE = "SING";
const QByteArray Commands::MODE_AUTO_FAST = "F_A_";

//Scope trigger edges
const QByteArray Commands::EDGE_RISING = "RISE";
const QByteArray Commands::EDGE_FALLING = "FALL";

//Scope sampling frequencies
const QByteArray Commands::FREQ_1K = "1K__";
const QByteArray Commands::FREQ_2K = "2K__";
const QByteArray Commands::FREQ_5K = "5K__";
const QByteArray Commands::FREQ_10K = "10K_";
const QByteArray Commands::FREQ_20K = "20K_";
const QByteArray Commands::FREQ_50K = "50K_";
const QByteArray Commands::FREQ_100K = "100K";
const QByteArray Commands::FREQ_200K = "200K";
const QByteArray Commands::FREQ_500K = "500K";
const QByteArray Commands::FREQ_1M = "1M__";
const QByteArray Commands::FREQ_2M = "2M__";
const QByteArray Commands::FREQ_5M = "5M__";
const QByteArray Commands::FREQ_10M = "10M_";
const QByteArray Commands::FREQ_MAX = "MAX_";

//Scope data lengths
const QByteArray Commands::SAMPLES_100 = "100_";
const QByteArray Commands::SAMPLES_200 = "200_";
const QByteArray Commands::SAMPLES_500 = "500_";
const QByteArray Commands::SAMPLES_1K = "1K__";
const QByteArray Commands::SAMPLES_2K = "2K__";
const QByteArray Commands::SAMPLES_5K = "5K__";
const QByteArray Commands::SAMPLES_10K = "10K_";
const QByteArray Commands::SAMPLES_20K = "20K_";
const QByteArray Commands::SAMPLES_50K = "50K_";
const QByteArray Commands::SAMPLES_100K = "100K";

//Scope Data depths
const QByteArray Commands::DATA_DEPTH_12B = "12B_";
const QByteArray Commands::DATA_DEPTH_10B = "10B_";
const QByteArray Commands::DATA_DEPTH_8B = "8B__";
const QByteArray Commands::DATA_DEPTH_6B = "6B__";

//Number of channels
const QByteArray Commands::CHANNELS_1 = "1CH_";
const QByteArray Commands::CHANNELS_2 = "2CH_";
const QByteArray Commands::CHANNELS_3 = "3CH_";
const QByteArray Commands::CHANNELS_4 = "4CH_";

/**************************** GENERATOR ****************************/
const QByteArray Commands::CMD_GEN_MODE = "MODE";
const QByteArray Commands::CMD_MODE_PWM = "PWM_";
const QByteArray Commands::CMD_MODE_DAC = "DAC_";
const QByteArray Commands::CMD_GEN_DATA = "DATA";
const QByteArray Commands::CMD_GEN_OK = "G_OK";
const QByteArray Commands::CMD_GEN_NEXT = "G_NX";
const QByteArray Commands::CMD_GEN_SAMPLING_FREQ = "FREQ";
const QByteArray Commands::CMD_GEN_OUTBUFF_ON =	"B_ON";
const QByteArray Commands::CMD_GEN_OUTBUFF_OFF = "B_OF";
const QByteArray Commands::CMD_GEN_DATA_LENGTH_CH1 = "LCH1";
const QByteArray Commands::CMD_GEN_DATA_LENGTH_CH2 = "LCH2";
const QByteArray Commands::CMD_GEN_CHANNELS = "CHAN";
const QByteArray Commands::CMD_GEN_START = "STRT";
const QByteArray Commands::CMD_GEN_STOP = "STOP";
const QByteArray Commands::CMD_GEN_RESET = "RSET";
const QByteArray Commands::CMD_GET_PWM_CONFIG = "PCF?";
const QByteArray Commands::CMD_GEN_DEINIT =	"GPDI";
//onst QByteArray Commands::CMD_GEN_PWM_FREQ_PSC = "FPWP";
//const QByteArray Commands::CMD_GEN_PWM_FREQ_ARR =	"FPWA";
const QByteArray Commands::CMD_GEN_PWM_FREQ_CH1 = "GFR1";
const QByteArray Commands::CMD_GEN_PWM_FREQ_CH2	= "GFR2";
const QByteArray Commands::CMD_GEN_DAC_VAL = "DAC_";
const QByteArray Commands::CMD_GEN_PWM_REAL_FREQ_CH1 = "GPR1";
const QByteArray Commands::CMD_GEN_PWM_REAL_FREQ_CH2 = "GPR2";
const QByteArray Commands::CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH1 = "GSR1";
const QByteArray Commands::CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH2 = "GSR2";

/************************ COUNTER ***********************/
// Counter complete deinit
const QByteArray Commands::COUNTER_DEINIT = "DEIN";

// Coutner common
const QByteArray Commands::QUANT_FREQUENCY = "QFRE";
const QByteArray Commands::QUANT_PERIOD = "QPER";

// Counter modes
const QByteArray Commands::COUNTER_MODE = "MODE";
const QByteArray Commands::MODE_HIGH_FREQ = "ETR_";
const QByteArray Commands::MODE_LOW_FREQ = "IC__";
const QByteArray Commands::MODE_RATIO = "RAT_";
const QByteArray Commands::MODE_INTERVAL = "TI__";

// Counter HF quantity
const QByteArray Commands::HF_QUANTITY = "QUAN";

// Counter HF GATE times
const QByteArray Commands::HF_GATE_TIME = "GATE";
const QByteArray Commands::HF_TIME_100M = "100m";
const QByteArray Commands::HF_TIME_500M = "500m";
const QByteArray Commands::HF_TIME_1S = "1s__";
const QByteArray Commands::HF_TIME_5S = "5s__";
const QByteArray Commands::HF_TIME_10S = "10s_";

// Counter LF quantity
const QByteArray Commands::LF_CH1_QUANTITY = "QIC1";
const QByteArray Commands::LF_CH2_QUANTITY = "QIC2";

// Counter LF channel multiplier
const QByteArray Commands::LF_CH1_MULTIPLIER = "PRE1";
const QByteArray Commands::LF_CH2_MULTIPLIER = "PRE2";

// Counter LF multiplier
const QByteArray Commands::LF_MULTIPLIER_1X = "1x__";
const QByteArray Commands::LF_MULTIPLIER_2X = "2x__";
const QByteArray Commands::LF_MULTIPLIER_4X = "4x__";
const QByteArray Commands::LF_MULTIPLIER_8X = "8x__";

// Counter LF Sample Count buffer
const QByteArray Commands::LF_CH1_SAMPLE_COUNT = "BUF1";
const QByteArray Commands::LF_CH2_SAMPLE_COUNT = "BUF2";

// Counter LF Duty Cycle
const QByteArray Commands::LF_DUTY_CYCLE = "DUCY";
const QByteArray Commands::LF_CH1_DUTY_CYCLE_ENABLE = "DCE1";
const QByteArray Commands::LF_CH2_DUTY_CYCLE_ENABLE = "DCE2";
const QByteArray Commands::LF_DUTY_CYCLE_DISABLE = "DCX_";

// Counter Ratio
const QByteArray Commands::RAT_CH3_SAMPLE_COUNT = "RSC_";

// Counter Intervals meas.
const QByteArray Commands::INT_TIMEOUT_SEC = "TIMO";

const QByteArray Commands::INT_EVENT = "EVNT";
const QByteArray Commands::INT_EVENT_RISE_FALL_CH1 = "RF1_";
const QByteArray Commands::INT_EVENT_RISE_FALL_CH2 = "RF2_";
const QByteArray Commands::INT_EVENT_RISE_CH1 = "RO1_";
const QByteArray Commands::INT_EVENT_RISE_CH2 = "RO2_";
const QByteArray Commands::INT_EVENT_FALL_CH1 = "FO1_";
const QByteArray Commands::INT_EVENT_FALL_CH2 = "FO2_";
const QByteArray Commands::INT_EVENT_SEQUENCE_AB = "SQAB";
const QByteArray Commands::INT_EVENT_SEQUENCE_BA = "SQBA";

/************************ SYNC PWM GENERATOR ***********************/
//Syncronized PWM generator general commands
const QByteArray Commands::SPWM_COMMAND = "SCOM";

const QByteArray Commands::SPWM_INIT = "INIT";
const QByteArray Commands::SPWM_DEINIT = "DINI";

//Set frequency command
const QByteArray Commands::SPWM_FREQ_CONFIG = "SPCF";
const QByteArray Commands::SPWM_DUTYPHASE_CONFIG = "SPDP";

//Individual channels run or not
const QByteArray Commands::SPWM_CHANNEL_INVERT = "SPCI";
const QByteArray Commands::SPWM_CHANNEL_STATE = "SSTA";
const int Commands::SPWM_CHANNEL_ENABLE = 1;
const int Commands::SPWM_CHANNEL_DISABLE = 0;

// One pulse mode to let the timer disable after overflow
const QByteArray Commands::SPWM_STEP_MODE = "STEP";
const QByteArray Commands::SPWM_STEP_ENABLE = "STEE";
const QByteArray Commands::SPWM_STEP_DISABLE = "STED";

