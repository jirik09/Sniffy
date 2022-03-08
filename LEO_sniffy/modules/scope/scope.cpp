#include "scope.h"

Scope::Scope(QObject *parent)
{
    Q_UNUSED(parent);
    config = new ScopeConfig();
    moduleSpecification = new ScopeSpec();
    specification = static_cast<ScopeSpec*>(moduleSpecification);
    measCalc = new MeasCalculations();
    mathCalc = new MathCalculations();
    FFTCalc = new FFTengine();
    scpWindow = new ScopeWindow(config);
    scpWindow->setObjectName("scpWindow");

    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = cmd->SCOPE;
    moduleName = "Oscilloscope";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_scope.png";

    scopeData = new QVector<QVector<QPointF>>;
    timeAndMemoryHandle = new TimeBaseAndMemory(config,this);

    //connect signals from GUI into scope
    connect(scpWindow, &ScopeWindow::timeBaseChanged,this,&Scope::updateTimebase);
    connect(scpWindow, &ScopeWindow::pretriggerChanged,this,&Scope::updatePretrigger);
    connect(scpWindow, &ScopeWindow::triggerValueChanged,this,&Scope::updateTriggerLevel);
    connect(scpWindow, &ScopeWindow::channelEnableChanged,this,&Scope::updateChannelsEnable);
    connect(scpWindow, &ScopeWindow::triggerModeChanged,this,&Scope::updateTriggerMode);
    connect(scpWindow, &ScopeWindow::triggerEdgeChanged,this,&Scope::updateTriggerEdge);
    connect(scpWindow, &ScopeWindow::triggerChannelChanged,this,&Scope::updateTriggerChannel);
    connect(scpWindow, &ScopeWindow::memoryPolicyChanged,this,&Scope::updateMemoryPolicy);
    connect(scpWindow, &ScopeWindow::memoryCustomLengthChanged,this,&Scope::updateMemoryCustomLength);
    connect(scpWindow, &ScopeWindow::samlingFrequecyCustomInputChanged,this,&Scope::updateCustomSamplingFreq);

    connect(scpWindow, &ScopeWindow::resolutionChanged,this,&Scope::updateResolution);

    connect(scpWindow, &ScopeWindow::measurementChanged,this, &Scope::addMeasurement);
    connect(scpWindow, &ScopeWindow::measurementClearChanged, this, &Scope::clearMeasurement);
    connect(measCalc, &MeasCalculations::measCalculated, this, &Scope::updateMeasurement);

    connect(scpWindow, &ScopeWindow::mathExpressionChanged,this,&Scope::updateMathExpression);
    connect(scpWindow, &ScopeWindow::fftChanged,this,&Scope::updateFFTConfig);
    connect(mathCalc, &MathCalculations::mathCalculated, this, &Scope::updateMath);
    connect(FFTCalc, &FFTengine::fftCalculated, this, &Scope::updateFFT);

    connect(timeAndMemoryHandle,&TimeBaseAndMemory::updateSamplingFrequency,this, &Scope::setSamplingFrequency);
    connect(timeAndMemoryHandle,&TimeBaseAndMemory::updateMemorySamplesLength,this, &Scope::setDataLength);
    connect(timeAndMemoryHandle,&TimeBaseAndMemory::updateNumChannels,this, &Scope::setNumberOfChannels);
    connect(timeAndMemoryHandle,&TimeBaseAndMemory::updateDataResolution,this, &Scope::setResolution);
}

void Scope::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        data.remove(0,4);
        moduleSpecification->parseSpecification(data);
        showModuleControl();
        buildModuleDescription(specification);
    }else if(dataHeader=="SMPL"){
        if(config->triggerMode!=ScopeTriggerMode::TRIG_STOP){
            scpWindow->samplingOngoing();
        }
    }else if(dataHeader=="TRIG"){
        scpWindow->triggerCaptured();

    }else if(dataHeader=="OSC_"){
        quint8 tmpByte;
        quint16 tmpShort;
        qreal minX = 0;
        qreal maxX = 0.1;
        qreal x(0);
        qreal y(0);
        int resolution;
        int length, samples;
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
            samples = resolution>8?length/2:length;
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
            config->numberOfChannelsReceived = numChannels;

            QVector<QPointF> points;
            if(length<500000){
                points.reserve(length);
            }else{
                qDebug() << "ERROR Too long signal to be alocated" << length;
            }

            if(resolution>8){
                minX = float(samples)*1.0/samplingFreq * ((100.0-config->pretriggerPercent)/100.0 - 1);
                config->timeMin = minX;
                for (int j(0); j < length/2; j++){
                    streamBuffLeng>>tmpByte;
                    tmpShort = tmpByte;
                    streamBuffLeng>>tmpByte;
                    tmpShort += tmpByte*256;
                    x=j*1.0/samplingFreq + minX;
                    y=(tmpShort*(float)(config->rangeMax-config->rangeMin)/(pow(2,config->ADCresolution)-1)+config->rangeMin)/1000;
                    points.append(QPointF(x,y));
                }
                maxX = x;
                config->timeMax = maxX;
            }else{
                minX = float(samples)*1.0/samplingFreq * ((100.0-config->pretriggerPercent)/100.0 - 1);
                config->timeMin = minX;
                for (int j(0); j < length; j++){
                    streamBuffLeng>>tmpByte;
                    tmpShort = tmpByte;
                    x=j*1.0/samplingFreq + minX;
                    y=(tmpShort*(float)(config->rangeMax-config->rangeMin)/(pow(2,config->ADCresolution)-1)+config->rangeMin)/1000;
                    points.append(QPointF(x,y));
                }
                maxX = x;
                config->timeMax = maxX;
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
            measCalc->calculate(*scopeData,config->scopeMeasList,config->realSamplingRate);
            mathCalc->calculate(*scopeData,config->realSamplingRate,mathExpression);
            if(FFTlength !=0 && numChannels>FFTChannelIndex){
                FFTCalc->calculate(scopeData->at(FFTChannelIndex),FFTwindow,FFTtype,FFTlength,true,config->realSamplingRate);
            }
            scpWindow->showDataTraces(*scopeData,config->timeBase, config->triggerChannelIndex);

            qreal zoomMultSampling = float(config->requestedSamplingRate)/config->realSamplingRate;//signal sometimes need to be zoomed to show correct value V/div
            qreal zoomMultLength = (float)samples/config->dataLength;
            scpWindow->setDataMinMaxTimeAndZoom(minX,maxX,zoomMultSampling*zoomMultLength*timeAndMemoryHandle->getZoom());
            scpWindow->setRealSamplingRateAndLlength(samplingFreq,samples);

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
    if(isConfigurationWritten)return;

    isConfigurationWritten = true;
    scpWindow->restoreGUIAfterStartup();
    scpWindow->setNumChannels(specification->maxADCChannels);
    scpWindow->setRealSamplingRateAndLlength(config->realSamplingRate,config->dataLength);

    comm->write(moduleCommandPrefix+":"+cmd->SCOPE_ADC_CHANNEL_DEAFULT+";");

    timeAndMemoryHandle->setMaxParams(specification->memorySize,specification->maxADCChannels);

    setResolution(config->ADCresolution);
    setDataLength(config->dataLength);
    setSamplingFrequency(config->requestedSamplingRate);
    updateTriggerLevel(config->triggerLevelPercent);
    updatePretrigger(config->pretriggerPercent);
    updateTriggerEdge(config->triggerEdge);
    setNumberOfChannels(config->numberOfChannels);
    updateTriggerChannel(config->triggerChannelIndex);
    if(config->triggerMode==ScopeTriggerMode::TRIG_STOP)
        config->triggerMode=ScopeTriggerMode::TRIG_SINGLE;
    setTriggerMode(config->triggerMode);
}

void Scope::parseConfiguration(QByteArray config){
    this->config->parse(config);
}
QByteArray Scope::getConfiguration(){
    return config->serialize();
}

void Scope::stopModule(){
    isConfigurationWritten = false;
    stopSampling();
    measCalc->exit();
    mathCalc->exit();
    FFTCalc->exit();
}
void Scope::startModule(){
    if(config->triggerMode != ScopeTriggerMode::TRIG_STOP)
        startSampling();
}

QWidget* Scope::getWidget(){
    return scpWindow;
}

void Scope::updateTimebase(float div){
    timeAndMemoryHandle->setTimeBase(div);
}

void Scope::updateCustomSamplingFreq(int freq)
{
    timeAndMemoryHandle->overWriteSamplingFreq(freq);
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
void Scope::updateMemoryPolicy(int memPolicyIndex){
    timeAndMemoryHandle->setMemoryPolicy((MemoryPolicy)memPolicyIndex);
}

void Scope::updateMemoryCustomLength(int length)
{
    timeAndMemoryHandle->overWriteMemoryLength(length);
}

void Scope::updateResolution(int resolution)
{
    timeAndMemoryHandle->setDataResolution(resolution);
}

void Scope::updateChannelsEnable(int buttonStatus){
    scpWindow->showDataTraces(*scopeData,config->timeBase,config->triggerChannelIndex);
    timeAndMemoryHandle->setNumOfChannels(fmax(log2(buttonStatus),0)+1);
}

void Scope::addMeasurement(Measurement *m){
    config->scopeMeasList.append(m);
    if(config->scopeMeasList.length()>9){
        config->scopeMeasList.removeFirst();
    }
    config->measCount= config->scopeMeasList.length();
    measCalc->calculate(*scopeData,config->scopeMeasList,config->realSamplingRate);
}

void Scope::clearMeasurement(){
    config->scopeMeasList.clear();
    config->measCount = 0;
    updateMeasurement(config->scopeMeasList);
}

void Scope::buildModuleDescription(ScopeSpec *spec)
{
    QString name = moduleName;
    QList<QString> labels ,values;

    labels.append("ADC Channels");
    values.append(QString::number(spec->maxADCChannels));

    labels.append("Memory size");
    values.append(LabelFormator::formatOutout(spec->memorySize,"B",1));

    labels.append("Max sampling rate");
    values.append(LabelFormator::formatOutout(spec->maxSamplingRate12B,"sps",2));

    labels.append("Pins");
    QString pins;
    for(int i = 0;i<spec->maxADCChannels;i++){
        pins += spec->channelPins[i] + ", ";
    }
    values.append(pins.left(pins.length()-2));

    showModuleDescription(name, labels, values);
}

void Scope::updateMeasurement(QList<Measurement*> m){
    scpWindow->updateMeasurement(m);
}

void Scope::updateMath(int errorPosition)
{
    scpWindow->updateMath(mathCalc->getCalculatedMathTrace());
    scpWindow->mathError(errorPosition);
}

void Scope::updateMathExpression(QString exp)
{
    mathExpression = exp;
    mathCalc->calculate(*scopeData,config->realSamplingRate,mathExpression);
}

void Scope::updateFFTConfig(int length, FFTWindow window, FFTType type, int channelIndex)
{
    FFTlength = length;
    FFTwindow = window;
    FFTtype = type;
    FFTChannelIndex = channelIndex;

    if(FFTlength !=0 && config->numberOfChannelsReceived>FFTChannelIndex){
        FFTCalc->calculate(scopeData->at(FFTChannelIndex),FFTwindow,FFTtype,FFTlength,true,config->realSamplingRate);
    }
}

void Scope::updateFFT()
{
    scpWindow->updateFFTchart(FFTCalc->getProcessedData(), FFTCalc->getMaxFrequency());
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

void Scope::setResolution(int res)
{
    if(res==8){
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_DATA_DEPTH+":"+cmd->DATA_DEPTH_8B+";");
    }else{
        comm->write(cmd->SCOPE+":"+cmd->SCOPE_DATA_DEPTH+":"+cmd->DATA_DEPTH_12B+";");
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
    if (mode == ScopeTriggerMode::TRIG_AUTO && (float)(config->dataLength)/config->requestedSamplingRate>0.5 ){
        mode = ScopeTriggerMode::TRIG_AUTO_FAST;
    }

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
