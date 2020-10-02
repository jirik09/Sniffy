#ifndef SCOPECONFIG_H
#define SCOPECONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "scopedefs.h"



class ScopeConfig : public QObject
{
    Q_OBJECT
public:
    explicit ScopeConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

    //all parameters must be initialized to default values and also aligned with GUI (I know it is not good)

    //parameters really in HW (received each time with data)

    int realSamplingRate = 1000;
    int ADCresolution = 12;
    int dataLength = 1200;
    int rangeMin = 0; //mV
    int rangeMax = 3300; //mv

    int numberOfChannels = 1;
    int triggerLevel = 32767; //16bit number
    int pretrigger = 32767; //16bit number


    //parameters for GUI
    int enabledChannels[MAX_SCOPE_CHANNELS] = {1,0 ,0, 0};
    float triggerLevelPercent = 50; //0-100% number
    float pretriggerPercent = 50; //0-100% number
    bool longestDataLength = false;
    bool signalMegazoom = false;
    int requestedSamplingRate = 10000;

    //params common for both
    ScopeTriggerMode triggerMode = ScopeTriggerMode::TRIG_AUTO;
    int triggerChannelIndex = 0;
    ScopeTriggerEdge triggerEdge = ScopeTriggerEdge::EDGE_RISING;
    float timeBase = 0.001;

    int selectedChannelIndexVertical = 0;
    float channelScale[MAX_SCOPE_CHANNELS]={1,1,1,1};
    float channelOffset[MAX_SCOPE_CHANNELS]={0,0,0,0};
    float channelScaleIndex[MAX_SCOPE_CHANNELS] = {-1,-1,-1,-1};
    float channelOffsetIndex[MAX_SCOPE_CHANNELS] = {-1,-1,-1,-1};


    //MCU data
    int memorySize = 0;


};

#endif // SCOPECONFIG_H
