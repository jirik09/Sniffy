#ifndef SCOPECONFIG_H
#define SCOPECONFIG_H

#include <QObject>
#include "scopedefs.h"



class ScopeConfig : public QObject
{
    Q_OBJECT
public:
    explicit ScopeConfig(QObject *parent = nullptr);


    //parameters physicaly for Hardware
    int samplingRate;
    int ADCresolution;
    int dataLength;
    int triggerLevel; //16bit number
    int pretrigger; //16bit number
    int numberOfChannels;


    //parameters for GUI
    int enabledChannels[MAX_SCOPE_CHANNELS];
    float triggerLevelPercent; //0-100% number
    float pretriggerPercent; //0-100% number


    //params common for both
    ScopeTriggerMode triggerMode;
    int triggerChannel;
    ScopeTriggerEdge triggerEdge;
    float timeBase;
    int longMemory;
    float verticalScale[MAX_SCOPE_CHANNELS];
    float verticalShift[MAX_SCOPE_CHANNELS];


signals:

};

#endif // SCOPECONFIG_H
