#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>



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


    static const QByteArray GENERATOR;
    static const QByteArray PWM_GENERATOR;
    static const QByteArray SYNC_PWM_GEN;
    static const QByteArray LOG_ANLYS;
    static const QByteArray COUNTER;
    static const QByteArray SCOPE;
    static const QByteArray SCOPE_INCOME;
    static const QByteArray COMMS;
    static const QByteArray SYSTEM;

    static const QByteArray STOP;
    static const QByteArray START;
    static const QByteArray NEXT;

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

    /************************ COUNTER ***********************/
    // Counter complete deinit
    static const QByteArray COUNTER_DEINIT;

    // Counter modes
    static const QByteArray COUNTER_MODE;
    static const QByteArray MODE_HIGH_FREQ;
    static const QByteArray MODE_LOW_FREQ;
    static const QByteArray MODE_REFERENCE;
    static const QByteArray MODE_INTERVAL;

    // Counter quantity
    static const QByteArray COUNTER_QUANTITY;
    static const QByteArray QUANT_FREQUENCY;
    static const QByteArray QUANT_PERIOD;

    // Counter ETR GATE times
    static const QByteArray COUNTER_GATE;
    static const QByteArray GATE_TIME_100M;
    static const QByteArray GATE_TIME_500M;
    static const QByteArray GATE_TIME_1S;
    static const QByteArray GATE_TIME_5S;
    static const QByteArray GATE_TIME_10S;
};

#endif // COMMANDS_H
