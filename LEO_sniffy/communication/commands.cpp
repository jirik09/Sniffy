#include "commands.h"

Commands::Commands(QObject *parent) : QObject(parent)
{
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

// Counter modes
const QByteArray Commands::COUNTER_MODE = "MODE";
const QByteArray Commands::MODE_HIGH_FREQ = "ETR_";
const QByteArray Commands::MODE_LOW_FREQ = "IC__";
const QByteArray Commands::MODE_REFERENCE = "REF_";
const QByteArray Commands::MODE_INTERVAL = "TI__";

// Counter quantity
const QByteArray Commands::COUNTER_QUANTITY = "QUAN";
const QByteArray Commands::QUANT_FREQUENCY = "QFRE";
const QByteArray Commands::QUANT_PERIOD = "QPER";

// Counter ETR GATE times
const QByteArray Commands::COUNTER_GATE = "GATE";
const QByteArray Commands::GATE_TIME_100M = "100m";
const QByteArray Commands::GATE_TIME_500M = "500m";
const QByteArray Commands::GATE_TIME_1S = "1s__";
const QByteArray Commands::GATE_TIME_5S = "5s__";
const QByteArray Commands::GATE_TIME_10S = "10s_";
