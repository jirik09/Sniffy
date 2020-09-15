#ifndef SCOPECONFIG_H
#define SCOPECONFIG_H

#include <QObject>
#include "scopedefs.h"



class ScopeConfig : public QObject
{
    Q_OBJECT
public:
    explicit ScopeConfig(QObject *parent = nullptr);

    //all parameters must be initialized to default values and also aligned with GUI (I know it is not good)

    //parameters really in HW (received each time with data)
    int samplingRate = 10000;
    int ADCresolution = 12;
    int dataLength = 1200;
    int numberOfChannels = 1;


    int triggerLevel = 32767; //16bit number
    int pretrigger = 32767; //16bit number


    //parameters for GUI
    int enabledChannels[MAX_SCOPE_CHANNELS] = {1,0 ,0, 0};
    float triggerLevelPercent = 50; //0-100% number
    float pretriggerPercent = 50; //0-100% number


    //params common for both
    ScopeTriggerMode triggerMode = ScopeTriggerMode::TRIG_AUTO;
    int triggerChannelIndex = 0;
    ScopeTriggerEdge triggerEdge = ScopeTriggerEdge::EDGE_RISING;
    float timeBase = 0.001;
    int longMemory = 0;
    float verticalScale[MAX_SCOPE_CHANNELS];
    float verticalShift[MAX_SCOPE_CHANNELS];


};

#endif // SCOPECONFIG_H
