#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>

class Commands
{
public:
const QByteArray IDN = "IDN_";

/* COMMUNICATION */
// General
const QByteArray IDN_REQUEST = "IDN?";
const QByteArray VERSION_REQUEST = "VER?";
const QByteArray CONFIG_REQUEST = "CFG?";
const QByteArray IS_SHIELD_CONNECTED = "SH_?";
const QByteArray RESET_DEVICE = "RES!";


const QByteArray GENERATOR = "GEN_";
const QByteArray PWM_GENERATOR = "GENP";
const QByteArray SYNC_PWM_GEN = "SYNP";
const QByteArray LOG_ANLYS = "LOGA";
const QByteArray COUNTER = "CNT_";
const QByteArray SCOPE = "OSCP";
const QByteArray SCOPE_INCOME = "OSC_";
const QByteArray COMMS = "COMS";
const QByteArray SYSTEM = "SYST";

const QByteArray GET_REAL_SMP_FREQ = "FRQ?";
const QByteArray GET_SCOPE_INPUTS = "INP?";


/************************ SCOPE ***********************/
//Scope
 const QByteArray CHANNELS = "CHAN";
 const QByteArray DATA_LENGTH = "LENG";  //number of samples

 const QByteArray DATA_LENGTH_CH1 = "LCH1";
 const QByteArray DATA_LENGTH_CH2 = "LCH2";

 const QByteArray SAMPLING_FREQ = "FREQ";

 const QByteArray TRIGGERED = "TRIG";
 const QByteArray SAMPLING = "SMPL";
 const QByteArray SCOPE_OK = "S_OK";
//Scope specific commands flags
 const QByteArray SCOPE_TRIG_MODE = "TRIG";
 const QByteArray SCOPE_TRIG_EDGE = "EDGE";
 const QByteArray SCOPE_TRIG_LEVEL = "LEVL";
 const QByteArray SCOPE_TRIG_CHANNEL = "TRCH";
 const QByteArray SCOPE_DATA_DEPTH = "DATA";
 const QByteArray SCOPE_PRETRIGGER = "PRET";
 const QByteArray SCOPE_NEXT = "NEXT";
 const QByteArray SCOPE_FREQ = "OSCF";
 const QByteArray SCOPE_ADC_CHANNEL = "A_CH";
 const QByteArray SCOPE_ADC_CHANNEL_DEAFULT = "ADEF";
 const QByteArray SCOPE_ADC_CHANNEL_VREF = "AREF";

//scope modes
 const QByteArray MODE_NORMAL = "NORM";
 const QByteArray MODE_AUTO = "AUTO";
 const QByteArray MODE_SINGLE = "SING";
 const QByteArray MODE_AUTO_FAST = "F_A_";

//Scope trigger edges

 const QByteArray EDGE_RISING = "RISE";
 const QByteArray EDGE_FALLING = "FALL";

//Scope sampling frequencies

 const QByteArray FREQ_1K = "1K__";
 const QByteArray FREQ_2K = "2K__";
 const QByteArray FREQ_5K = "5K__";
 const QByteArray FREQ_10K = "10K_";
 const QByteArray FREQ_20K = "20K_";
 const QByteArray FREQ_50K = "50K_";
 const QByteArray FREQ_100K = "100K";
 const QByteArray FREQ_200K = "200K";
 const QByteArray FREQ_500K = "500K";
 const QByteArray FREQ_1M = "1M__";
 const QByteArray FREQ_2M = "2M__";
 const QByteArray FREQ_5M = "5M__";
 const QByteArray FREQ_10M = "10M_";
 const QByteArray FREQ_MAX = "MAX_";

//Scope data lengths

 const QByteArray SAMPLES_100 = "100_";
 const QByteArray SAMPLES_200 = "200_";
 const QByteArray SAMPLES_500 = "500_";
 const QByteArray SAMPLES_1K = "1K__";
 const QByteArray SAMPLES_2K = "2K__";
 const QByteArray SAMPLES_5K = "5K__";
 const QByteArray SAMPLES_10K = "10K_";
 const QByteArray SAMPLES_20K = "20K_";
 const QByteArray SAMPLES_50K = "50K_";
 const QByteArray SAMPLES_100K = "100K";

//Scope Data depths

 const QByteArray DATA_DEPTH_12B = "12B_";
 const QByteArray DATA_DEPTH_10B = "10B_";
 const QByteArray DATA_DEPTH_8B = "8B__";
 const QByteArray DATA_DEPTH_6B = "6B__";

//Number of channels

 const QByteArray CHANNELS_1 = "1CH_";
 const QByteArray CHANNELS_2 = "2CH_";
 const QByteArray CHANNELS_3 = "3CH_";
 const QByteArray CHANNELS_4 = "4CH_";

};

#endif // COMMANDS_H
