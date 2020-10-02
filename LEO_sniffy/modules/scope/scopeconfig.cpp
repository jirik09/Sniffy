#include "scopeconfig.h"

ScopeConfig::ScopeConfig(QObject *parent) : QObject(parent)
{

}

void ScopeConfig::parse(QByteArray config)
{
    QDataStream stream(config);
    stream >> realSamplingRate;// = 1000;
    stream >> ADCresolution;// = 12;
    stream >> dataLength;// = 1200;
    stream >> rangeMin;// = 0; //mV
    stream >> rangeMax;// = 3300; //mv

    stream >> numberOfChannels;// = 1;
    stream >> triggerLevel;// = 32767; //16bit number
    stream >> pretrigger;// = 32767; //16bit number

    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream >> enabledChannels[i];
    }
    //parameters for GUI
    stream >> triggerLevelPercent;// = 50; //0-100% number
    stream >> pretriggerPercent;// = 50; //0-100% number
    stream >> longestDataLength;// = false;
    stream >> signalMegazoom;// = false;
    stream >> requestedSamplingRate;// = 10000;

    //params common for both
    stream >> triggerMode;// = ScopeTriggerMode::TRIG_AUTO;
    stream >> triggerChannelIndex;// = 0;
    stream >> triggerEdge;// = ScopeTriggerEdge::EDGE_RISING;
    stream >> timeBase;// = 0.001;

    stream >> selectedChannelIndexVertical;

    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream >> channelScale[i];
    }
    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream >> channelOffset[i];
    }
    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream >> channelOffsetIndex[i];
    }
    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream >> channelScaleIndex[i];
    }

   // qDebug() << "Cofig from scope is being parsed "+config;
}

QByteArray ScopeConfig::serialize()
{
    QByteArray *data;
    data = new QByteArray();
    QDataStream stream(data,QIODevice::WriteOnly);

    stream << realSamplingRate;// = 1000;
    stream << ADCresolution;// = 12;
    stream << dataLength;// = 1200;
    stream << rangeMin;// = 0; //mV
    stream << rangeMax;// = 3300; //mv

    stream << numberOfChannels;// = 1;
    stream << triggerLevel;// = 32767; //16bit number
    stream << pretrigger;// = 32767; //16bit number

    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream << enabledChannels[i];
    }
    //parameters for GUI
    stream << triggerLevelPercent;// = 50; //0-100% number
    stream << pretriggerPercent;// = 50; //0-100% number
    stream << longestDataLength;// = false;
    stream << signalMegazoom;// = false;
    stream << requestedSamplingRate;// = 10000;

    //params common for both
    stream << triggerMode;// = ScopeTriggerMode::TRIG_AUTO;
    stream << triggerChannelIndex;// = 0;
    stream << triggerEdge;// = ScopeTriggerEdge::EDGE_RISING;
    stream << timeBase;// = 0.001;

    stream << selectedChannelIndexVertical;

    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream << channelScale[i];
    }
    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream << channelOffset[i];
    }
    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream << channelOffsetIndex[i];
    }
    for(int i = 0; i<MAX_SCOPE_CHANNELS;i++){
        stream << channelScaleIndex[i];
    }
 //   qDebug() << "Cofig from scope is being serialized "+*data;
    return *data;

}


