#ifndef SCOPECONFIG_H
#define SCOPECONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "scopedefs.h"

#include "measurement.h"

#include "../abstractconfiguration.h"

class ScopeConfig : public AbstractConfiguration
{
    Q_OBJECT
public:
    explicit ScopeConfig(QObject *parent = nullptr);

    void parse (QByteArray config);
    QByteArray serialize ();

    //all parameters must be initialized to default values and also aligned with GUI (I know it is not good)

    //parameters really in HW (received each time with data)
    int realSamplingRate;
    int ADCresolution;
    int dataLength = DEFAULT_MEM_SAMPLES_LENGTH;
    int rangeMin; //mV
    int rangeMax; //mv

    int numberOfChannels = 1;
    int numberOfChannelsReceived = 0;
    int triggerLevel = 32767; //16bit number
    int pretrigger = 32767; //16bit number


    //parameters for GUI
    float triggerLevelPercent = 50; //0-100% number
    float pretriggerPercent = 50; //0-100% number
    int requestedSamplingRate = 100000;

    //params common for both
    ScopeTriggerMode triggerMode = ScopeTriggerMode::TRIG_AUTO;
    int triggerChannelIndex = 0;
    ScopeTriggerEdge triggerEdge = ScopeTriggerEdge::EDGE_RISING;
    float timeBase = 0.001;
    MemoryPolicy memPolicy = MemoryPolicy::NORMAL;


    //parems from GUI
    int selectedChannelIndexVertical = 0;
    float channelScale[MAX_SCOPE_CHANNELS+1]={1,1,1,1,1};
    float channelOffset[MAX_SCOPE_CHANNELS+1]={0,0,0,0,0};
    float channelScaleIndex[MAX_SCOPE_CHANNELS+1] = {-1,-1,-1,-1,-1};
    float channelOffsetIndex[MAX_SCOPE_CHANNELS+1] = {-1,-1,-1,-1,-1};
    int cursorChannelIndex = 0;
    int cursorsActiveIndex = 0;
    float timeMin = -0.01;
    float timeMax = 0.01;

    int measCount = 0;
    QList<Measurement *> scopeMeasList;
    bool FFTenabled = false;
    bool FFTisLog = false;

};

#endif // SCOPECONFIG_H
