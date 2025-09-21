#include "scopeconfig.h"
#include <QDataStream>
#include <QIODevice>

ScopeConfig::ScopeConfig(QObject *parent)
{
    Q_UNUSED(parent);
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

    //parameters for GUI
    stream >> triggerLevelPercent;// = 50; //0-100% number
    stream >> pretriggerPercent;// = 50; //0-100% number
    stream >> memPolicy;
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

    stream >> cursorChannelIndex;
    stream >> cursorsActiveIndex;
    stream >> FFTenabled;
    stream >> timeMin;// = -0.01;
    stream >> timeMax;// = 0.01;
    stream >> measCount;

    MeasurementType type;
    QString label;
    qint32 channelIndex;

    scopeMeasList.clear();
    for(int i = 0; i<measCount;i++){
        stream >> type;
        stream >> label;
        stream >> channelIndex;
        Measurement *m = new Measurement(type,channelIndex);
        m->setLabel(label);
        scopeMeasList.append(m);
    }
    isConfigurationLoaded = true;

   // qDebug() << "Cofig from scope is being parsed "+config;
}

QByteArray ScopeConfig::serialize()
{
    QByteArray data; // stack allocated avoids leak
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << realSamplingRate;// = 1000;
    stream << ADCresolution;// = 12;
    stream << dataLength;// = 1200;
    stream << rangeMin;// = 0; //mV
    stream << rangeMax;// = 3300; //mv

    stream << numberOfChannels;// = 1;
    stream << triggerLevel;// = 32767; //16bit number
    stream << pretrigger;// = 32767; //16bit number

    //parameters for GUI
    stream << triggerLevelPercent;// = 50; //0-100% number
    stream << pretriggerPercent;// = 50; //0-100% number
    stream << memPolicy;
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

    stream << cursorChannelIndex;
    stream << cursorsActiveIndex;
    stream << FFTenabled;
    stream << timeMin;// = -0.01;
    stream << timeMax;// = 0.01;
    stream << measCount;
    for(int i = 0; i<measCount;i++){
        stream << scopeMeasList[i]->getType();
        stream << scopeMeasList[i]->getLabel();
        stream << scopeMeasList[i]->getChannelIndex();
    }


 //   qDebug() << "Cofig from scope is being serialized "+data;
     return data;

}


