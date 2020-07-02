#include "scope.h"

Scope::Scope(QObject *parent)
{
    Q_UNUSED(parent);
    config = new ScopeConfig();
    measCalc = new MeasCalculations();
    cmd = new Commands();
    setCommandPrefix(cmd->SCOPE);
    setModuleName("Oscilloscope");
    scpWindow = new WindowScope();

    scopeData = new QVector<QVector<QPointF>>;
    //scopeMeas = new QList<Measurement>;

    //connect signals from GUI into scope
    connect(scpWindow, &WindowScope::timeBaseChanged,this,&Scope::updateTimebase);
    connect(scpWindow, &WindowScope::pretriggerChanged,this,&Scope::updatePretrigger);
    connect(scpWindow, &WindowScope::triggerValueChanged,this,&Scope::updateTriggerLevel);
    connect(scpWindow, &WindowScope::channelEnableChanged,this,&Scope::updateChannelsEnable);
    connect(scpWindow, &WindowScope::triggerModeChanged,this,&Scope::updateTriggerMode);
    connect(scpWindow, &WindowScope::triggerEdgeChanged,this,&Scope::updateTriggerEdge);
    connect(scpWindow, &WindowScope::triggerChannelChanged,this,&Scope::updateTriggerChannel);

    connect(scpWindow, &WindowScope::measurementChanged,this, &Scope::addMeasurement);
    connect(scpWindow, &WindowScope::measurementClearChanged, this, &Scope::clearMeasurement);
    connect(measCalc, &MeasCalculations::measCalculated, this, &Scope::updateMeasurement);
}

void Scope::parseData(QByteArray data){
 //   qDebug() << "data are in scope parser" << data;

    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        moduleControlWidget->show();
        //todo pass specification into scopeSpec.cpp and parse it

    }else if(dataHeader=="SMPL"){
        scpWindow->samplingOngoing();

    }else if(dataHeader=="TRIG"){
        scpWindow->triggerCaptured();

    }else if(dataHeader=="OSC_"){
        qint8 tmpByte;
        qint16 tmpShort;
        qreal minX;
        qreal maxX;
        qreal x(0);
        qreal y(0);
        int resolution;
        int length;
        int samplingFreq;
        int currentChannel=0;
        int numChannels=1;

        QDataStream streamBuffLeng(data);

        while (numChannels>currentChannel){
            streamBuffLeng>>samplingFreq; //thow away first 4 bytes because it contains "OSC_"

            streamBuffLeng>>samplingFreq;
            config->samplingRate = samplingFreq;

            streamBuffLeng>>tmpByte;
            resolution = tmpByte;
            config->ADCresolution = resolution;

            streamBuffLeng>>length;
            resolution>8?config->dataLength = length/2:config->dataLength = length;

            streamBuffLeng>>tmpByte;
            streamBuffLeng>>tmpByte;
            currentChannel = tmpByte;

            streamBuffLeng>>tmpByte;
            numChannels = tmpByte;
            config->numberOfChannels = numChannels;

            QVector<QPointF> points;
            if(length<500000){
                points.reserve(length);
            }else{
                qDebug() << "ERROR Too long signal to be alocated" << length;
            }

            if(resolution>8){
                minX = float(config->dataLength)*1.0/samplingFreq * ((100.0-config->pretriggerPercent)/100.0 - 1);
                for (int j(0); j < length/2; j++){
                    streamBuffLeng>>tmpByte;
                    tmpShort = tmpByte;
                    streamBuffLeng>>tmpByte;
                    tmpShort += tmpByte*256;
                    x = 0;
                    y = 0;
                    x=j*1.0/samplingFreq + minX;
                    y=tmpShort*3.3/4095.0;
                    points.append(QPointF(x,y));
                }
                maxX = x;
            }else{
                qDebug() << "TODO <8bit data parser";
            }

            while(numChannels<scopeData->length()){ //remove signals which will not be filled
                scopeData->removeAt(scopeData->length()-1);
            }

            if(scopeData->length()>=currentChannel){
                scopeData->replace(currentChannel-1,points);
            }else{
                scopeData->append(points);
            }
        }

        if(currentChannel==numChannels){
            measCalc->calculate(*scopeData,scopeMeas,config->samplingRate);
            scpWindow->showDataTraces(*scopeData,config->timeBase, config->triggerChannelIndex);
            scpWindow->setDataMinMaxTime(minX,maxX);
            scpWindow->setRealSamplingRate(samplingFreq);

            //handel single trigger
            if(config->triggerMode!=ScopeTriggerMode::TRIG_SINGLE){
                restartSampling();
            }else{
                scpWindow->singleSamplingDone();
                moduleControlWidget->setStatus(ModuleStatus::PAUSE);
            }
        }
    }else{
        qDebug() << "ERROR such data cannot be parsed in Scope";
    }
}

void Scope::writeConfiguration(){
    updateTimebase(config->timeBase);

    //this one will be done like others
    config->dataLength = 1000;
    comm->write(cmd->SCOPE+":"+cmd->DATA_LENGTH+":"+cmd->SAMPLES_1K+";");
    config->longMemory = 0;

    updateTriggerLevel(config->triggerLevelPercent);
    updatePretrigger(config->pretriggerPercent);

    updateTriggerMode(config->triggerMode);
    updateTriggerEdge(config->triggerEdge);

    updateTriggerChannel(config->triggerChannelIndex);
    updateChannelsEnable(1);
}

void Scope::stopModule(){
    stopSampling();
    measCalc->exit();
}
void Scope::startModule(){
    startSampling();
}

QWidget* Scope::getWidget(){
    return scpWindow;
}

void Scope::updateTimebase(float div){
    config->timeBase = div;
    config->samplingRate = round(float(config->dataLength)/(20.0*div)+0.49);
    setSamplingFrequency(config->samplingRate);
}

void Scope::updatePretrigger(float percentage){
    config->pretriggerPercent=percentage;
    config->pretrigger = 65535*percentage/100;
    comm->write(cmd->SCOPE, cmd->SCOPE_PRETRIGGER, config->pretrigger);
}

void Scope::updateTriggerLevel(float percentage){
    config->triggerLevelPercent=percentage;
    config->triggerLevel = 65535*percentage/100;
    comm->write(cmd->SCOPE, cmd->SCOPE_TRIG_LEVEL, config->triggerLevel);
}

void Scope::updateTriggerMode(ScopeTriggerMode mode){
    config->triggerMode=mode;
    setTriggerMode(mode);

    if(mode==ScopeTriggerMode::TRIG_STOP){
        stopSampling();
    }else{
        startSampling();
    }
}

void Scope::updateTriggerEdge(ScopeTriggerEdge edge){
    setTriggerEdge(edge);
    config->triggerEdge=edge;
}

void Scope::updateTriggerChannel(int index){
    config->triggerChannelIndex=index;
    setTriggerChannel(index);
}

void Scope::updateChannelsEnable(int buttonStatus){
    if(buttonStatus & 0x01){
        config->enabledChannels[0]=1;
        config->numberOfChannels = 1;
    }
    if(buttonStatus & 0x02){
        config->enabledChannels[1]=1;
        config->numberOfChannels = 2;
    }
    if(buttonStatus & 0x04){
        config->enabledChannels[2]=1;
        config->numberOfChannels = 3;
    }
    if(buttonStatus & 0x08){
        config->enabledChannels[3]=1;
        config->numberOfChannels = 4;
    }
    setNumberOfChannels(config->numberOfChannels);
    scpWindow->paintTraces(*scopeData);
}

void Scope::addMeasurement(Measurement *m){
    scopeMeas.append(m);
    if(scopeMeas.length()>9){
        scopeMeas.removeFirst();
    }
    measCalc->calculate(*scopeData,scopeMeas,config->samplingRate);
}

void Scope::clearMeasurement(){
    scopeMeas.clear();
    updateMeasurement(scopeMeas);
}

void Scope::updateMeasurement(QList<Measurement*> m){
    scpWindow->updateMeasurement(m);
}

void Scope::stopSampling(){
    comm->write(cmd->SCOPE+":"+cmd->STOP+";");
    moduleControlWidget->setStatus(ModuleStatus::PAUSE);
}

void Scope::startSampling(){
    comm->write(cmd->SCOPE+":"+cmd->START+";");
    moduleControlWidget->setStatus(ModuleStatus::PLAY);
}

// ******************* Private functions - not important and no logic inside *******
void Scope::restartSampling(){
    comm->write(cmd->SCOPE+":"+cmd->NEXT+";");
}

void Scope::setNumberOfChannels(int num){
    switch (num){
    case 1:
        comm->write(cmd->SCOPE, cmd->CHANNELS, cmd->CHANNELS_1);
        break;
    case 2:
        comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_2+";");
        break;
    case 3:
        comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_3+";");
        break;
    case 4:
        comm->write(cmd->SCOPE+":"+cmd->CHANNELS+":"+cmd->CHANNELS_4+";");
        break;
    }
}


void Scope::setTriggerChannel(int chan){
    if(chan==0)
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_1+";");
    if(chan==1)
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_2+";");
    if(chan==2)
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_3+";");
    if(chan==3)
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_CHANNEL+":"+cmd->CHANNELS_4+";");
}

void Scope::setTriggerMode(ScopeTriggerMode mode){
    switch(mode){
    case ScopeTriggerMode::TRIG_SINGLE:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_SINGLE+";");
        break;
    case ScopeTriggerMode::TRIG_AUTO:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_AUTO+";");
        break;
    case ScopeTriggerMode::TRIG_NORMAL:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_NORMAL+";");
        break;
    case ScopeTriggerMode::TRIG_STOP:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_SINGLE+";");
        break;
    case ScopeTriggerMode::TRIG_AUTO_FAST:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_AUTO_FAST+";");
        break;
    }
}

void Scope::setTriggerEdge(ScopeTriggerEdge edge){
    switch (edge) {
    case ScopeTriggerEdge::EDGE_RISING:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_EDGE+":"+cmd->EDGE_RISING+";");
        break;
    case ScopeTriggerEdge::EDGE_FALLING:
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_EDGE+":"+cmd->EDGE_FALLING+";");
        break;
    }
}

void Scope::setSamplingFrequency(int samplingRate){
    qDebug () << samplingRate;
    comm->write(cmd->SCOPE,cmd->SAMPLING_FREQ,samplingRate);

   /* if(samplingRate<=1000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_1K+";");
    }else if(samplingRate<=2000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_2K+";");
    }else if(samplingRate<=5000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_5K+";");
    }else if(samplingRate<=10000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_10K+";");
    }else if(samplingRate<=20000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_20K+";");
    }else if(samplingRate<=50000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_50K+";");
    }else if(samplingRate<=100000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_100K+";");
    }else if(samplingRate<=200000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_200K+";");
    }else if(samplingRate<=500000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_500K+";");
    }else if(samplingRate<=1000000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_1M+";");
    }else if(samplingRate<=2000000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_2M+";");
    }else if(samplingRate<=5000000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_5M+";");
    }else if(samplingRate<=10000000){
        comm->write(cmd->SCOPE+":"+cmd->SAMPLING_FREQ+":"+cmd->FREQ_10M+";");
    }*/
}
