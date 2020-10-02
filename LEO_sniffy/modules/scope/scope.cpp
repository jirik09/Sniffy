#include "scope.h"

Scope::Scope(QObject *parent)
{
    Q_UNUSED(parent);
    config = new ScopeConfig();
    specification = new ScopeSpec();
    measCalc = new MeasCalculations();
    scpWindow = new ScopeWindow();
    scpWindow->setObjectName("scpWindow");
    scpWindow->passConfig(*config);

    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = cmd->SCOPE;
    moduleName = "Oscilloscope";
    moduleIconURI = ":/graphics/graphics/icon_scope.png";


    scopeData = new QVector<QVector<QPointF>>;

    //connect signals from GUI into scope
    connect(scpWindow, &ScopeWindow::timeBaseChanged,this,&Scope::updateTimebase);
    connect(scpWindow, &ScopeWindow::pretriggerChanged,this,&Scope::updatePretrigger);
    connect(scpWindow, &ScopeWindow::triggerValueChanged,this,&Scope::updateTriggerLevel);
    connect(scpWindow, &ScopeWindow::channelEnableChanged,this,&Scope::updateChannelsEnable);
    connect(scpWindow, &ScopeWindow::triggerModeChanged,this,&Scope::updateTriggerMode);
    connect(scpWindow, &ScopeWindow::triggerEdgeChanged,this,&Scope::updateTriggerEdge);
    connect(scpWindow, &ScopeWindow::triggerChannelChanged,this,&Scope::updateTriggerChannel);
    connect(scpWindow, &ScopeWindow::memoryLengthChanged,this,&Scope::updateMemoryLength);

    connect(scpWindow, &ScopeWindow::measurementChanged,this, &Scope::addMeasurement);
    connect(scpWindow, &ScopeWindow::measurementClearChanged, this, &Scope::clearMeasurement);
    connect(measCalc, &MeasCalculations::measCalculated, this, &Scope::updateMeasurement);

}

void Scope::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        data.remove(0,4);
        specification->parseSpecification(data);
        showModuleControl();

    }else if(dataHeader=="SMPL"){
        if(config->triggerMode!=ScopeTriggerMode::TRIG_STOP){
            scpWindow->samplingOngoing();
        }
    }else if(dataHeader=="TRIG"){
        scpWindow->triggerCaptured();

    }else if(dataHeader=="OSC_"){
        quint8 tmpByte;
        quint16 tmpShort;
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
            streamBuffLeng>>samplingFreq;
            streamBuffLeng>>samplingFreq;
            config->realSamplingRate = samplingFreq;
            streamBuffLeng>>tmpByte;
            resolution = tmpByte;
            config->ADCresolution = resolution;
            streamBuffLeng>>length;
            config->dataLength = resolution>8?length/2:length;
            streamBuffLeng>>tmpShort;
            config->rangeMin = (qint16)tmpShort;
            streamBuffLeng>>tmpShort;
            config->rangeMax = (qint16)tmpShort;

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
                    y=(tmpShort*(float)(config->rangeMax-config->rangeMin)/(pow(2,config->ADCresolution)-1)+config->rangeMin)/1000;
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
            measCalc->calculate(*scopeData,scopeMeas,config->realSamplingRate);
            scpWindow->showDataTraces(*scopeData,config->timeBase, config->triggerChannelIndex);

            //signal sometimes need to be zoomed to show correct value V/div
            float zoomMultSampling = float(config->requestedSamplingRate)/config->realSamplingRate;
            float zoomMultLength = config->signalMegazoom==true?1:(float)config->dataLength/1200;
            scpWindow->setDataMinMaxTimeAndZoom(minX,maxX,zoomMultSampling*zoomMultLength);
            scpWindow->setRealSamplingRate(samplingFreq);


            //handle single trigger
            if(config->triggerMode!=ScopeTriggerMode::TRIG_SINGLE){
                restartSampling();
            }else{
                scpWindow->singleSamplingDone();
                setModuleStatus(ModuleStatus::PAUSE);
            }
        }
    }else{
        qDebug() << "ERROR such data cannot be parsed in Scope";
    }
}

void Scope::writeConfiguration(){
    updateTimebase(config->timeBase);

    //this one will be done like others
    setDataLength(config->dataLength);

    updateTriggerLevel(config->triggerLevelPercent);
    updatePretrigger(config->pretriggerPercent);

    updateTriggerEdge(config->triggerEdge);

    updateTriggerChannel(config->triggerChannelIndex);

    setNumberOfChannels(config->numberOfChannels);
    updateTriggerMode(config->triggerMode);

}

void Scope::parseConfiguration(QByteArray config){
    this->config->parse(config);
}
QByteArray Scope::getConfiguration(){
    return config->serialize();
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
    if(config->signalMegazoom==true){
        config->requestedSamplingRate = round(float(config->dataLength)/(12.0*div)+0.49);
        qDebug() << config->requestedSamplingRate;
    }else{
        config->requestedSamplingRate = round(float(100)/(div)+0.49);
    }
    setSamplingFrequency(config->requestedSamplingRate);
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
void Scope::updateMemoryLength(int length){
    if(length==0){
        config->dataLength = 1200;
        config->longestDataLength = false;
        config->signalMegazoom = false;
    }
    if(length>0 && length<100){
        config->longestDataLength = true;
        config->signalMegazoom = length==2?true:false;

        if(config->ADCresolution>8){
            config->dataLength = specification->memorySize/config->numberOfChannels/2;
        }else{
            config->dataLength = specification->memorySize/config->numberOfChannels;
        }
    }
    updateTimebase(config->timeBase);
    setDataLength(config->dataLength);
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
    if(config->longestDataLength == true){
        setDataLength(1200);
        setNumberOfChannels(config->numberOfChannels);
        if(config->signalMegazoom==true){
            updateMemoryLength(2);
        }else{
            updateMemoryLength(1);
        }
    }else{
        setNumberOfChannels(config->numberOfChannels);
    }
    scpWindow->paintTraces(*scopeData);
}

void Scope::addMeasurement(Measurement *m){
    scopeMeas.append(m);
    if(scopeMeas.length()>9){
        scopeMeas.removeFirst();
    }
    measCalc->calculate(*scopeData,scopeMeas,config->realSamplingRate);
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
    setModuleStatus(ModuleStatus::PAUSE);
}

void Scope::startSampling(){
    comm->write(cmd->SCOPE+":"+cmd->START+";");
    setModuleStatus(ModuleStatus::PLAY);
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
    comm->write(cmd->SCOPE,cmd->SAMPLING_FREQ,samplingRate);
}

void Scope::setDataLength(int dataLength){
    comm->write(cmd->SCOPE,cmd->DATA_LENGTH,dataLength);
}
