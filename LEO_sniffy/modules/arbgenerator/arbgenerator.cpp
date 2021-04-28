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
    }else if(dataHeader==cmd->CMD_GEN_NEXT){
        if(!dataBeingUploaded)return;

        if (lengthToSend == 0){
            if (sendingChannel == 1 && numChannelsUsed == 2) {
                lengthToSend = GeneratorData[1].length();
                lengthSent = 0;
                memoryIndex = 0;
                actualSend = 0;
                sendingChannel = 2;
                sendNextData();
            }else if (sendingChannel == numChannelsUsed) {
                genAskForFreq();
                startGenerator();
            }
        }else {
            sendNextData();
        }
        arbGenWindow->setProgress(totalSent*100/totalToSend);

    }else if(dataHeader==cmd->CMD_GEN_OK){
        if(dataBeingUploaded){
            dataBeingUploaded = false;
            arbGenWindow->setGeneratorRuning();
        }
    }else if(dataHeader==cmd->CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH1){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setFrequencyLabels(0,(qreal)(freq)/signalLengths[0]);

    }else if(dataHeader==cmd->CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH2){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setFrequencyLabels(1,(qreal)(freq)/signalLengths[1]);

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
        setGeneratorPWMMode();
    }else{
        setGeneratorDACMode();
    }
    setModuleStatus(ModuleStatus::PAUSE);
}

void ArbGenerator::stopModule()
{
    stopGenerator();
    generatorDeinit();
    arbGenWindow->setGeneratorStopped();
}

void ArbGenerator::sendSignalCallback(){
    dataBeingUploaded = true;
    GeneratorData = *arbGenWindow->getGeneratorDACData();
    numChannelsUsed = GeneratorData.length();
    totalToSend = totalSent = 0;

    //Memory demand (#Chan * length) has to be reduced first
    if (numChannelsUsed > 1) {
        for(int i = 0;i<numChannelsUsed;i++){
            signalLengths[i] = GeneratorData[i].length();
            setDataLength(i, signalLengths[i]);
            totalToSend +=  signalLengths[i];
        }
        setNumChannels(numChannelsUsed);
    }else{
        setNumChannels(numChannelsUsed);
        signalLengths[0] = GeneratorData[0].length();
        setDataLength(0,signalLengths[0]);
        totalToSend =  signalLengths[0];
    }


    for(int i = 0;i<numChannelsUsed;i++){
        setSamplingFrequency(i,arbGenWindow->getFrequency(i)*signalLengths[i]);
    }

    if(isPWMbased){
        for(int i = 0;i<numChannelsUsed;i++){
            setPWMFrequency(i,arbGenWindow->getPWMFrequency(i));
        }
    }

    lengthToSend = signalLengths[0];
    lengthSent = memoryIndex = actualSend = 0;
    sendingChannel = 1;
    sendNextData();
}

void ArbGenerator::stopCallback()
{
    stopGenerator();
    dataBeingUploaded = false;
}

void ArbGenerator::updateFrequencyCallback() //this function counts only with two channels (logic for more channels as far more complex)
{
    if(arbGenWindow->getFrequency(1) != arbGenWindow->getFrequency(0) || signalLengths[0] != signalLengths[1]){
        for (int i = 0;i<numChannelsUsed;i++){
            setSamplingFrequency(i,arbGenWindow->getFrequency(i)*signalLengths[i]);
        }
    }else{
        setSamplingFrequency(99,arbGenWindow->getFrequency(0)*signalLengths[0]);
    }
    genAskForFreq();
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
        for(int i = 0;i<spec->maxDACChannels;i++){
            pins += spec->channelPWMPins[i] + ", ";
        }
        values.append(pins.left(pins.length()-2));
    }
    showModuleDescription(name, labels, values);
}

void ArbGenerator::sendNextData()
{  
    comm->write(moduleCommandPrefix+":"+cmd->CMD_GEN_DATA + " ");
    if (lengthToSend > SEND_BLOCK_SIZE){
        actualSend = SEND_BLOCK_SIZE;
    }else{
        actualSend = lengthToSend;
    }

    QByteArray tmpHeader;
    QDataStream dataStreamHeader(&tmpHeader, QIODevice::WriteOnly);
    int tmp = ((memoryIndex / 256)*256*256*256 + (memoryIndex % 256) * 256*256 + (actualSend * 256) + (sendingChannel));
    dataStreamHeader << tmp;
    comm->write(tmpHeader+":");

    QByteArray tmpData;
    QDataStream dataStreamData(&tmpData, QIODevice::WriteOnly);
    for (int i = 0; i < actualSend; i++){
        qint16  sample = qFromBigEndian<qint16>(GeneratorData[sendingChannel-1][lengthSent+i]);
        dataStreamData <<sample;
    }
    comm->write(tmpData+";");
    lengthSent += actualSend;
    lengthToSend -= actualSend;
    memoryIndex += actualSend;
    totalSent += actualSend;
}

void ArbGenerator::startGenerator()
{
    setModuleStatus(ModuleStatus::PLAY);
    comm->write(moduleCommandPrefix,cmd->START);
}

void ArbGenerator::stopGenerator()
{
    setModuleStatus(ModuleStatus::PAUSE);
    comm->write(moduleCommandPrefix,cmd->CMD_GEN_STOP);
}

void ArbGenerator::setGeneratorDACMode()
{
    comm->write(moduleCommandPrefix,cmd->CMD_GEN_MODE,cmd->CMD_MODE_DAC);
}

void ArbGenerator::setGeneratorPWMMode()
{
    comm->write(moduleCommandPrefix,cmd->CMD_GEN_MODE,cmd->CMD_MODE_PWM);
}

void ArbGenerator::generatorDeinit()
{
    comm->write(moduleCommandPrefix,cmd->DEINIT);
}

void ArbGenerator::setDataLength(int channel, int length)
{
    if(channel==0){
        comm->write(moduleCommandPrefix,cmd->DATA_LENGTH_CH1,length);
    }else if(channel==1){
        comm->write(moduleCommandPrefix,cmd->DATA_LENGTH_CH2,length);
    }else{
        qDebug () << "More channels not yet implemented";
    }
}

void ArbGenerator::setNumChannels(int numChannels)
{
    if(numChannels==1){
        comm->write(moduleCommandPrefix,cmd->CHANNELS,cmd->CHANNELS_1);
    }else if(numChannels==2){
        comm->write(moduleCommandPrefix,cmd->CHANNELS,cmd->CHANNELS_2);
    }else{
        qDebug () << "More channels not yet implemented";
    }
}

void ArbGenerator::setSamplingFrequency(int channel, qreal freq)
{
    int tmp = (int)(round(freq))*256 + (channel+1)%256;
    int maxSamplingRate = static_cast<ArbGeneratorSpec*>(moduleSpecification)->maxSamplingRate;

    if(round(freq) > maxSamplingRate){
        tmp = maxSamplingRate*256 + (channel+1)%256;
    }
    comm->write(moduleCommandPrefix,cmd->SAMPLING_FREQ,tmp);
}

void ArbGenerator::setPWMFrequency(int channel, qreal freq)
{
    if(channel==0){
        comm->write(moduleCommandPrefix,cmd->CMD_GEN_PWM_FREQ_CH1,freq);
    }else{
        comm->write(moduleCommandPrefix,cmd->CMD_GEN_PWM_FREQ_CH2,freq);
    }
}

void ArbGenerator::genAskForFreq()
{
    comm->write(moduleCommandPrefix,cmd->GET_REAL_SMP_FREQ);
}

void ArbGenerator::setOutputBuffer(bool isEnabled)
{
    if(isEnabled){
        comm->write(moduleCommandPrefix,cmd->CMD_GEN_OUTBUFF_ON);
    }else{
        comm->write(moduleCommandPrefix,cmd->CMD_GEN_OUTBUFF_OFF);
    }
}



