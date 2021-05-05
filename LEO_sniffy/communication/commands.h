#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QList>

class Commands : public QObject
{
    Q_OBJECT
public:
    explicit Commands(QObject *parent = nullptr);

    static const QByteArray IDN;

    /* COMMUNICATION */
    // General
    static const QByteArray IDN_REQUEST;
    static const QByteArray VERSION_REQUEST;
    static const QByteArray CONFIG_REQUEST;
    static const QByteArray IS_SHIELD_CONNECTED;
    static const QByteArray RESET_DEVICE;


    static const QByteArray SIGNAL_GENERATOR;
    static const QByteArray VOLTAGE_SOURCE;
    static const QByteArray PWM_GENERATOR;
    static const QByteArray PATTERN_GENERATOR;
    static const QByteArray SYNC_PWM_GEN;    
    static const QByteArray LOG_ANLYS;
    static const QByteArray COUNTER;
    static const QByteArray SCOPE;
    static const QByteArray SCOPE_INCOME;
    static const QByteArray COMMS;
    static const QByteArray SYSTEM;

    static const QByteArray START;
    static const QByteArray STOP;
    static const QByteArray PAUSE;
    static const QByteArray UNPAUSE;
    static const QByteArray NEXT;
    static const QByteArray CONFIG;

    static const QByteArray GET_REAL_SMP_FREQ;
    static const QByteArray GET_SCOPE_INPUTS;


    /************************ SCOPE ***********************/
    //Scope
    static const QByteArray CHANNELS;
    static const QByteArray DATA_LENGTH;  //number of samples

    static const QByteArray DATA_LENGTH_CH1;
    static const QByteArray DATA_LENGTH_CH2;

    static const QByteArray SAMPLING_FREQ;

    static const QByteArray TRIGGERED;
    static const QByteArray SAMPLING;
    static const QByteArray SCOPE_OK;
    //Scope specific commands flags
    static const QByteArray SCOPE_TRIG_MODE;
    static const QByteArray SCOPE_TRIG_EDGE;
    static const QByteArray SCOPE_TRIG_LEVEL;
    static const QByteArray SCOPE_TRIG_CHANNEL;
    static const QByteArray SCOPE_DATA_DEPTH;
    static const QByteArray SCOPE_PRETRIGGER;
    static const QByteArray SCOPE_NEXT;
    static const QByteArray SCOPE_FREQ;
    static const QByteArray SCOPE_ADC_CHANNEL;
    static const QByteArray SCOPE_ADC_CHANNEL_DEAFULT;
    static const QByteArray SCOPE_ADC_CHANNEL_VREF;

    //scope modes
    static const QByteArray MODE_NORMAL;
    static const QByteArray MODE_AUTO;
    static const QByteArray MODE_SINGLE;
    static const QByteArray MODE_AUTO_FAST;

    //Scope trigger edges

    static const QByteArray EDGE_RISING;
    static const QByteArray EDGE_FALLING;

    //Scope sampling frequencies

    static const QByteArray FREQ_1K;
    static const QByteArray FREQ_2K;
    static const QByteArray FREQ_5K;
    static const QByteArray FREQ_10K;
    static const QByteArray FREQ_20K;
    static const QByteArray FREQ_50K;
    static const QByteArray FREQ_100K;
    static const QByteArray FREQ_200K;
    static const QByteArray FREQ_500K;
    static const QByteArray FREQ_1M;
    static const QByteArray FREQ_2M;
    static const QByteArray FREQ_5M;
    static const QByteArray FREQ_10M;
    static const QByteArray FREQ_MAX;

    //Scope data lengths

    static const QByteArray SAMPLES_100;
    static const QByteArray SAMPLES_200;
    static const QByteArray SAMPLES_500;
    static const QByteArray SAMPLES_1K;
    static const QByteArray SAMPLES_2K;
    static const QByteArray SAMPLES_5K;
    static const QByteArray SAMPLES_10K;
    static const QByteArray SAMPLES_20K;
    static const QByteArray SAMPLES_50K;
    static const QByteArray SAMPLES_100K;

    //Scope Data depths

    static const QByteArray DATA_DEPTH_12B;
    static const QByteArray DATA_DEPTH_10B;
    static const QByteArray DATA_DEPTH_8B;
    static const QByteArray DATA_DEPTH_6B;

    //Number of channels

    static const QByteArray CHANNELS_1;
    static const QByteArray CHANNELS_2;
    static const QByteArray CHANNELS_3;
    static const QByteArray CHANNELS_4;

    /**************************** GENERATOR ****************************/
    static const QByteArray CMD_GEN_MODE;
    static const QByteArray CMD_MODE_PWM;
    static const QByteArray CMD_MODE_DAC;
    static const QByteArray CMD_MODE_VOLT;
    static const QByteArray CMD_MODE_PATTERN;
    static const QByteArray CMD_GEN_DATA;
    static const QByteArray CMD_GEN_OK;
    static const QByteArray CMD_GEN_NEXT;
    static const QByteArray CMD_GEN_SAMPLING_FREQ;
    static const QByteArray CMD_GEN_OUTBUFF_ON;
    static const QByteArray CMD_GEN_OUTBUFF_OFF ;
    static const QByteArray CMD_GEN_DATA_LENGTH_CH1;
    static const QByteArray CMD_GEN_DATA_LENGTH_CH2;
    static const QByteArray CMD_GEN_CHANNELS;
    static const QByteArray CMD_GEN_START;
    static const QByteArray CMD_GEN_STOP;
    static const QByteArray CMD_GEN_RESET;
    static const QByteArray CMD_GET_PWM_CONFIG;
    static const QByteArray DEINIT;
    //static const QByteArray CMD_GEN_PWM_FREQ_PSC;
    //static const QByteArray CMD_GEN_PWM_FREQ_ARR;
    static const QByteArray CMD_GEN_PWM_FREQ_CH1;
    static const QByteArray CMD_GEN_PWM_FREQ_CH2;
    static const QByteArray CMD_GEN_DAC_VAL;
    static const QByteArray CMD_GEN_PWM_REAL_FREQ_CH1;
    static const QByteArray CMD_GEN_PWM_REAL_FREQ_CH2;
    static const QByteArray CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH1;
    static const QByteArray CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH2;

    /************************ COUNTER ***********************/
    // Counter complete deinit
    static const QByteArray COUNTER_DEINIT;

    // Counter modes
    static const QByteArray COUNTER_MODE;
    static const QByteArray MODE_HIGH_FREQ;
    static const QByteArray MODE_LOW_FREQ;
    static const QByteArray MODE_RATIO;
    static const QByteArray MODE_INTERVAL;    

    // Counter common
    static const QByteArray QUANT_FREQUENCY;
    static const QByteArray QUANT_PERIOD;

    // Counter HF quantity
    static const QByteArray HF_QUANTITY;

    // Counter HF GATE times
    static const QByteArray HF_GATE_TIME;
    static const QByteArray HF_TIME_100M;
    static const QByteArray HF_TIME_500M;
    static const QByteArray HF_TIME_1S;
    static const QByteArray HF_TIME_5S;
    static const QByteArray HF_TIME_10S;

    // Counter LF quantity
    static const QByteArray LF_CH1_QUANTITY;
    static const QByteArray LF_CH2_QUANTITY;

    // Counter LF channel multiplier
    static const QByteArray LF_CH1_MULTIPLIER;
    static const QByteArray LF_CH2_MULTIPLIER;

    // Counter LF multiplier
    static const QByteArray LF_MULTIPLIER_1X;
    static const QByteArray LF_MULTIPLIER_2X;
    static const QByteArray LF_MULTIPLIER_4X;
    static const QByteArray LF_MULTIPLIER_8X;

    // Counter LF Sample Count buffer
    static const QByteArray LF_CH1_SAMPLE_COUNT;
    static const QByteArray LF_CH2_SAMPLE_COUNT;

    // Counter LF Duty Cycle
    static const QByteArray LF_DUTY_CYCLE;
    static const QByteArray LF_CH1_DUTY_CYCLE_ENABLE;
    static const QByteArray LF_CH2_DUTY_CYCLE_ENABLE;
    static const QByteArray LF_DUTY_CYCLE_DISABLE;

    // Counter Ratio
    static const QByteArray RAT_CH3_SAMPLE_COUNT;

    // Counter Intervals
    static const QByteArray INT_TIMEOUT_SEC;

    static const QByteArray INT_EVENT;
    static const QByteArray INT_EVENT_RISE_FALL_CH1;
    static const QByteArray INT_EVENT_RISE_FALL_CH2;
    static const QByteArray INT_EVENT_RISE_CH1;
    static const QByteArray INT_EVENT_RISE_CH2;
    static const QByteArray INT_EVENT_FALL_CH1;
    static const QByteArray INT_EVENT_FALL_CH2;
    static const QByteArray INT_EVENT_SEQUENCE_AB;
    static const QByteArray INT_EVENT_SEQUENCE_BA;

    /************************ SYNC PWM GENERATOR ***********************/
    //Syncronized PWM generator general commands
    static const QByteArray SPWM_COMMAND;

    static const QByteArray SPWM_INIT;
    static const QByteArray SPWM_DEINIT;

    //Set frequency command
    static const QByteArray SPWM_FREQ_CONFIG;
    static const QByteArray SPWM_DUTYPHASE_CONFIG;

    //Individual channels run or not
    static const QByteArray SPWM_CHANNEL_INVERT;
    static const QByteArray SPWM_CHANNEL_STATE;
    static const int SPWM_CHANNEL_ENABLE;
    static const int SPWM_CHANNEL_DISABLE;

    // One pulse mode to let the timer disable after overflow
    static const QByteArray SPWM_STEP_MODE;
    static const QByteArray SPWM_STEP_ENABLE;
    static const QByteArray SPWM_STEP_DISABLE;
};

#endif // COMMANDS_H
