#include "arbgenerator.h"

ArbGenerator::ArbGenerator(QObject *parent, bool isPWMbased):
    isPWMbased(isPWMbased)
{
    Q_UNUSED(parent);
    moduleSpecification = new ArbGeneratorSpec();
    config = new ArbGeneratorConfig();
    arbGenWindow = new ArbGeneratorWindow(config, isPWMbased);

    if(isPWMbased){
        moduleCommandPrefix = cmd->PWM_GENERATOR;
        arbGenWindow->setObjectName("pwmGenWindow");
        moduleName = "PWM genarator";
        moduleIconURI = Graphics::getGraphicsPath()+"icon_pwm_gen.png";
    }else{
        moduleCommandPrefix = cmd->SIGNAL_GENERATOR;
        arbGenWindow->setObjectName("arbGenWindow");
        moduleName = "Arbitrary generator";
        moduleIconURI = Graphics::getGraphicsPath()+"icon_signal_generator.png";
    }

    connect(arbGenWindow, &ArbGeneratorWindow::runGenerator, this,&ArbGenerator::sendSignalCallback);
    connect(arbGenWindow, &ArbGeneratorWindow::stopGenerator, this,&ArbGenerator::stopCallback);
    connect(arbGenWindow, &ArbGeneratorWindow::updateFrequency, this,&ArbGenerator::updateFrequencyCallback);
    connect(arbGenWindow, &ArbGeneratorWindow::restartGenerating, this,&ArbGenerator::quickRestartCalback);
}

QWidget *ArbGenerator::getWidget()
{
    return arbGenWindow;
}
int test1 = 0, test2 = 0, text3 = 0;
void ArbGenerator::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader==cmd->CONFIG){
        data.remove(0,4);
        if(isPWMbased){
                static_cast<ArbGeneratorSpec*>(moduleSpecification)->parsePWMSpecification(data);
                arbGenWindow->setSpecification(static_cast<ArbGeneratorSpec*>(moduleSpecification));
                showModuleControl();
                buildModuleDescription(static_cast<ArbGeneratorSpec*>(moduleSpecification));
        }else{
                moduleSpecification->parseSpecification(data);
                arbGenWindow->setSpecification(static_cast<ArbGeneratorSpec*>(moduleSpecification));
                showModuleControl();
                buildModuleDescription(static_cast<ArbGeneratorSpec*>(moduleSpecification));
        }
        genComms = new GenCommons(moduleCommandPrefix,comm,static_cast<ArbGeneratorSpec*>(moduleSpecification)->maxSamplingRate,this);
    }else if(dataHeader==cmd->CMD_GEN_NEXT){
        if(!dataBeingUploaded)return;
        genComms->sendNext();
        if(genComms->isSentAll()){
            genComms->genAskForFreq();
            startGenerator();
        }
        arbGenWindow->setProgress(genComms->getProgress());

    }else if(dataHeader==cmd->CMD_GEN_OK){
        if(dataBeingUploaded){
            dataBeingUploaded = false;
            arbGenWindow->setGeneratorRuning();
        }
    }else if(dataHeader==cmd->CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH1){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setFrequencyLabels(0,(qreal)(freq)/genComms->getSignaLength(0));

    }else if(dataHeader==cmd->CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH2){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setFrequencyLabels(1,(qreal)(freq)/genComms->getSignaLength(1));

    }else if(dataHeader==cmd->CMD_GEN_PWM_REAL_FREQ_CH1){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setPWMFrequencyLabels(0,(qreal)(freq));

    }else if(dataHeader==cmd->CMD_GEN_PWM_REAL_FREQ_CH2){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setPWMFrequencyLabels(1,(qreal)(freq));

    }else{
        qDebug()<<"UNHANDLED"<<data;
    }
}

void ArbGenerator::writeConfiguration()
{
    arbGenWindow->restoreGUIAfterStartup();
}

void ArbGenerator::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray ArbGenerator::getConfiguration()
{
    return config->serialize();
}

void ArbGenerator::startModule()
{
    if(isPWMbased){
        genComms->setGeneratorPWMMode();
    }else{
        genComms->setGeneratorDACMode();
    }
    setModuleStatus(ModuleStatus::PAUSE);
}

void ArbGenerator::stopModule()
{
    stopGenerator();
    genComms->generatorDeinit();
    arbGenWindow->setGeneratorStopped();
}

void ArbGenerator::sendSignalCallback(){
    dataBeingUploaded = true;
    genComms->setSignaltoSend(arbGenWindow->getGeneratorDACData());
    numChannelsUsed = arbGenWindow->getGeneratorDACData()->length();

    for(int i = 0;i<numChannelsUsed;i++){
        genComms->setSamplingFrequency(i,arbGenWindow->getFrequency(i)*genComms->getSignaLength(i));
    }

    if(isPWMbased){
        for(int i = 0;i<numChannelsUsed;i++){
            genComms->setPWMFrequency(i,arbGenWindow->getPWMFrequency(i));
        }
    }
    genComms->sendNext();
}

void ArbGenerator::stopCallback()
{
    stopGenerator();
    dataBeingUploaded = false;
}

void ArbGenerator::updateFrequencyCallback() //this function counts only with two channels (logic for more channels as far more complex)
{
    if(arbGenWindow->getFrequency(1) != arbGenWindow->getFrequency(0) || genComms->getSignaLength(0) != genComms->getSignaLength(1)){
        for (int i = 0;i<numChannelsUsed;i++){
            genComms->setSamplingFrequency(i,arbGenWindow->getFrequency(i)*genComms->getSignaLength(i));
        }
    }else{
        genComms->setSamplingFrequency(99,arbGenWindow->getFrequency(0)*genComms->getSignaLength(0));
    }
    genComms->genAskForFreq();
}

void ArbGenerator::quickRestartCalback()
{
    stopGenerator();
    startGenerator();
}

void ArbGenerator::buildModuleDescription(ArbGeneratorSpec *spec)
{
    QString name = moduleName;
    QList<QString> labels ,values;
    if(!isPWMbased) {
        labels.append("DAC Channels");
        values.append(QString::number(spec->maxDACChannels));

        labels.append("DAC Resolution");
        values.append(QString::number(spec->DACResolution));

        labels.append("Memory size");
        values.append(LabelFormator::formatOutout(spec->generatorBufferSize,"B",1));

        labels.append("Max sampling rate");
        values.append(LabelFormator::formatOutout(spec->maxSamplingRate,"sps",2));

        labels.append("Pins");
        QString pins;
        for(int i = 0;i<spec->maxDACChannels;i++){
            pins += spec->channelPins[i] + ", ";
        }
        values.append(pins.left(pins.length()-2));

    }else{
        labels.append("PWM Channels");
        values.append(QString::number(spec->maxPWMChannels));

        labels.append("Timer clock");
        values.append(LabelFormator::formatOutout(spec->periphPWMClockFrequency,"Hz",1));

        labels.append("Pins");
        QString pins;
        for(int i = 0;i<spec->maxPWMChannels;i++){
            pins += spec->channelPWMPins[i] + ", ";
        }
        values.append(pins.left(pins.length()-2));
    }
    showModuleDescription(name, labels, values);
}

void ArbGenerator::startGenerator()
{
    setModuleStatus(ModuleStatus::PLAY);
    genComms->startGenerator();
}

void ArbGenerator::stopGenerator()
{
    setModuleStatus(ModuleStatus::PAUSE);
    genComms->stopGenerator();
}




