#include "voltmeter.h"

Voltmeter::Voltmeter(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new VoltmeterSpec();
    config = new VoltmeterConfig();
    voltWindow = new VoltmeterWindow(config);
    voltWindow->setObjectName("voltWindow");

    //Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = cmd->SCOPE;
    moduleName = "Voltmeter";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_voltmeter.png";

    measCalc = new MeasCalculations();
    voltmeterData = new QVector<QVector<QPointF>>;

    for(int i = 0;i<MAX_VOLTMETER_CHANNELS;i++){
        voltMeasList.append(new Measurement(MeasurementType::MEAN,i));
        voltMeasList.append(new Measurement(MeasurementType::RMS,i));
        voltMeasList.append(new Measurement(MeasurementType::PKPK,i));
        voltMeasList.append(new Measurement(MeasurementType::FREQUENCY,i));
    }

    //In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &Voltmeter::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &Voltmeter::holdButtonCallback);

    connect(measCalc, &MeasCalculations::measCalculated, this, &Voltmeter::updateMeasurementGUI);

    connect(voltWindow, &VoltmeterWindow::averagingChanged, this,&Voltmeter::setAveraging);
    connect(voltWindow, &VoltmeterWindow::numChannelsEnabledChanged, this,&Voltmeter::setNumChannelsEnabled);
    connect(voltWindow, &VoltmeterWindow::resetMinMax, this,&Voltmeter::resetMinMax);
    connect(voltWindow, &VoltmeterWindow::modeChanged, this,&Voltmeter::setMode);

}

QWidget *Voltmeter::getWidget()
{
    return voltWindow;
}

void Voltmeter::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        data.remove(0,4);
        moduleSpecification->parseSpecification(data);
        showModuleControl();
        //TODO parse message from MCU
    }else if(dataHeader=="OSC_"){
        quint8 tmpByte;
        quint16 tmpShort;
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
                for (int j(0); j < length/2; j++){
                    streamBuffLeng>>tmpByte;
                    tmpShort = tmpByte;
                    streamBuffLeng>>tmpByte;
                    tmpShort += tmpByte*256;
                    x=j*1.0/samplingFreq;
                    y=(tmpShort*(float)(config->rangeMax-config->rangeMin)/(pow(2,config->ADCresolution)-1)+config->rangeMin)/1000;
                    points.append(QPointF(x,y));
                }
            }else{
                qDebug() << "TODO <8bit data parser";
            }

            while(numChannels<voltmeterData->length()){ //remove signals which will not be filled
                voltmeterData->removeAt(voltmeterData->length()-1);
            }

            if(voltmeterData->length()>=currentChannel){
                voltmeterData->replace(currentChannel-1,points);
            }else{
                voltmeterData->append(points);
            }
        }

        if(currentChannel==numChannels){
            measCalc->calculate(*voltmeterData,voltMeasList,config->realSamplingRate);
        }
    }else{
        //qDebug()<<data;
    }
}

void Voltmeter::writeConfiguration()
{
    if(isConfigurationWritten)return;

    isConfigurationWritten = true;
    //workaround first data was corrupted on channels > 1
    numChannelsEnabled = MAX_VOLTMETER_CHANNELS;
    setDefaultSampling();

    voltWindow->restoreGUIAfterStartup();

    voltWindow->setPins(static_cast<VoltmeterSpec*>(moduleSpecification)->channelPins,MAX_VOLTMETER_CHANNELS);
    setVDDSampling();
    comm->write(moduleCommandPrefix,cmd->SAMPLING_FREQ,5000);
    comm->write(cmd->SCOPE,cmd->DATA_LENGTH,200);
    comm->write(cmd->SCOPE+":"+cmd->SCOPE_TRIG_MODE+":"+cmd->MODE_AUTO_FAST+";");
    comm->write(cmd->SCOPE+":"+cmd->SCOPE_DATA_DEPTH+":"+cmd->DATA_DEPTH_12B+";");
}

void Voltmeter::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray Voltmeter::getConfiguration()
{
    return config->serialize();
}

void Voltmeter::startModule()
{
    startSampling();
}

void Voltmeter::stopModule()
{
    isConfigurationWritten = false;
    voltWindow->stopDatalog();
    stopSampling();
}

//In case hold is needed
void Voltmeter::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void Voltmeter::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->STOP+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->START+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}
void Voltmeter::updateSamplingChannel(void){
    if(isReferenceMeasured){
        setDefaultSampling();
    }else{
        switch (config->mode){
        case VoltmeterMode::NORMAL:
            config->targetDataLength = 200;
            setVDDSampling();
            break;
        case VoltmeterMode::FAST:
            config->targetDataLength = 200;
            setDefaultSampling();
            break;
        }
    }
}

void Voltmeter::updateMeasurementGUI(QList<Measurement *> m)
{
    if(isReferenceMeasured){
        updateSamplingChannel();
        qreal VrefReal = static_cast<VoltmeterSpec*>(moduleSpecification)->VrefCalibration / (pow(2,config->ADCresolution)-1) * static_cast<VoltmeterSpec*>(moduleSpecification)->Vref;
        realVdd = VrefReal / m.at(0)->getValue() * static_cast<VoltmeterSpec*>(moduleSpecification)->Vref/1000;
        voltWindow->showVddValue(qreal(int(realVdd))/1000);

    }else{
        samplesTaken++;
        voltWindow->showProgress(samplesTaken,samplesToTakeTotal);
        foreach(Measurement* meas, m){
            if(meas->getType() == MeasurementType::MEAN){
                dataRawVoltage[meas->getChannelIndex()].append(meas->getValue()*(realVdd/static_cast<VoltmeterSpec*>(moduleSpecification)->Vref));
            }
        }

        if(samplesTaken>=samplesToTakeTotal || isStartup == true){
            updateSamplingChannel();
            samplesTaken = 0;
            isStartup = false;
            for(int i=0;i<MAX_VOLTMETER_CHANNELS;i++){
                data[i].voltage =  getAverage(&dataRawVoltage[i]);
                data[i].percent = (data[i].voltage*1000+config->rangeMin)*100/(config->rangeMax-config->rangeMin);
                if(data[i].voltage>data[i].max){
                    data[i].max = data[i].voltage;
                }
                if(data[i].voltage<data[i].min){
                    data[i].min = data[i].voltage;
                }
            }
            foreach(Measurement* meas, m){
                if(meas->getType() == MeasurementType::PKPK){
                    data[meas->getChannelIndex()].ripple = meas->getValue();
                }
                if(meas->getType() == MeasurementType::FREQUENCY && meas->getLabel()!="N/A"){
                    data[meas->getChannelIndex()].frequency = meas->getValue();
                }
            }

            for(int i=0;i<MAX_VOLTMETER_CHANNELS;i++){
                dataRawVoltage[i].clear();
            }

            voltWindow->showData(data,numChannelsEnabled);
        }
    }
    restartSampling();
    voltWindow->showProgress(samplesTaken,samplesToTakeTotal);
}


qreal Voltmeter::getAverage(QList<qreal> *list)
{
    qreal out = 0;
    if (list->length()>=1){
        foreach(qreal num, *list){
            out += num;
        }
        out = out/list->length();
    }
    return out;
}

void Voltmeter::setAveraging(int value)
{
    samplesToTakeTotal = value;
}

void Voltmeter::setNumChannelsEnabled(int value)
{
    numChannelsEnabled = value;
}

void Voltmeter::resetMinMax()
{
    for(int i=0;i<MAX_VOLTMETER_CHANNELS;i++){
        data[i].max = -10000;
        data[i].min = 10000;
    }
}

void Voltmeter::setMode(int index)
{
    switch (index){
    case 0:
        config->mode = VoltmeterMode::NORMAL;
        break;
    case 1:
        config->mode = VoltmeterMode::FAST;
        break;
    default:
        config->mode = VoltmeterMode::NORMAL;
        break;
    }
}


// ******************* Private functions - not important and no logic inside *******
void Voltmeter::stopSampling(){
    setModuleStatus(ModuleStatus::PAUSE);
    comm->write(cmd->SCOPE+":"+cmd->STOP+";");
}

void Voltmeter::startSampling(){
    setModuleStatus(ModuleStatus::PLAY);
    comm->write(cmd->SCOPE+":"+cmd->START+";");
}

void Voltmeter::restartSampling(){
    comm->write(cmd->SCOPE+":"+cmd->NEXT+";");
}

void Voltmeter::setNumberOfChannels(int num){
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

void Voltmeter::setVDDSampling(){
    comm->write(moduleCommandPrefix+":"+cmd->SCOPE_ADC_CHANNEL_VREF+";");
    int length = 400 + +samplesToTakeTotal*100;
    if(length>2000){
        length = 2000;
    }
    comm->write(cmd->SCOPE,cmd->DATA_LENGTH,length);
    setNumberOfChannels(1);
    isReferenceMeasured = true;
}

void Voltmeter::setDefaultSampling()
{
    comm->write(moduleCommandPrefix+":"+cmd->SCOPE_ADC_CHANNEL_DEAFULT+";");
    comm->write(cmd->SCOPE,cmd->DATA_LENGTH,config->targetDataLength);
    setNumberOfChannels(numChannelsEnabled);
    isReferenceMeasured = false;
}


