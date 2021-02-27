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
    moduleIconURI = ":/graphics/graphics/icon_voltmeter.png";

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

    connect(measCalc, &MeasCalculations::measCalculated, this, &Voltmeter::updateMeasurement);


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
            // config->realSamplingRate = samplingFreq;
            streamBuffLeng>>tmpByte;
            resolution = tmpByte;
            config->ADCresolution = resolution;
            //config->ADCresolution = resolution;
            streamBuffLeng>>length;
            config->dataLength = resolution>8?length/2:length;
            //config->dataLength = resolution>8?length/2:length;
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
            //scpWindow->showDataTraces(*scovoltmeterDatapeData,config->timeBase, config->triggerChannelIndex);
        }
    }
    else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void Voltmeter::writeConfiguration()
{
    voltWindow->restoreGUIAfterStartup();

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

void Voltmeter::updateMeasurement(QList<Measurement *> m)
{
    if(isReferenceMeasured){
        qDebug () << "reference";
        setDefaultSampling();
        qreal VrefReal = static_cast<VoltmeterSpec*>(moduleSpecification)->VrefCalibration / (pow(2,config->ADCresolution)-1) * static_cast<VoltmeterSpec*>(moduleSpecification)->Vref;
        realVdd = VrefReal / m.at(0)->getValue() * static_cast<VoltmeterSpec*>(moduleSpecification)->Vref/1000;
        qDebug () << realVdd;

        for(int i=0;i<MAX_VOLTMETER_CHANNELS;i++){
            dataRawVoltage[i].clear();
        }

    }else{
        qDebug () << "data";
        samplesTaken++;
        int chIndex = 0;
        foreach(Measurement* meas, m){
            if(meas->getType() == MeasurementType::MEAN){
                dataRawVoltage[chIndex].append(meas->getValue());
                chIndex++;
            }
        }

        if(samplesTaken>=samplesToTakeTotal){
            samplesTaken = 0;
            for(int i=0;i<MAX_VOLTMETER_CHANNELS;i++){
                dataVoltage[i] =  getAverage(&dataRawVoltage[i]);
                qDebug () << dataVoltage[i];
            }
            setVDDSampling();
        }
    }



    restartSampling();
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
    //   setNumberOfChannels(1);
    isReferenceMeasured = true;
}

void Voltmeter::setDefaultSampling()
{
    comm->write(moduleCommandPrefix+":"+cmd->SCOPE_ADC_CHANNEL_DEAFULT+";");
    //    setNumberOfChannels(numChannelsEnabled);
    isReferenceMeasured = false;
}


