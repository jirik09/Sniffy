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
const QByteArray Commands::SYNC_PWM_GEN = "SYNP";
const QByteArray Commands::LOG_ANLYS = "LOGA";
const QByteArray Commands::COUNTER = "CNT_";
const QByteArray Commands::SCOPE = "OSCP";
const QByteArray Commands::SCOPE_INCOME = "OSC_";
const QByteArray Commands::COMMS = "COMS";
const QByteArray Commands::SYSTEM = "SYST";

const QByteArray Commands::STOP = "STOP";
const QByteArray Commands::START = "STRT";
const QByteArray Commands::NEXT = "NEXT";

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

