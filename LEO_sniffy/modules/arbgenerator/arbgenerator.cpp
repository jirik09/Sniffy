#include "arbgenerator.h"

ArbGenerator::ArbGenerator(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new ArbGeneratorSpec();
    config = new ArbGeneratorConfig();
    arbGenWindow = new ArbGeneratorWindow(config);
    arbGenWindow->setObjectName("arbGenWindow");

    moduleCommandPrefix = cmd->GENERATOR;
    moduleName = "Arbitrary generator";
    moduleIconURI = ":/graphics/graphics/icon_signal_generator.png";

    connect(arbGenWindow, &ArbGeneratorWindow::runGenerator, this,&ArbGenerator::sendSignalCallback);
    connect(arbGenWindow, &ArbGeneratorWindow::stopGenerator, this,&ArbGenerator::stopCallback);
    connect(arbGenWindow, &ArbGeneratorWindow::updateFrequency, this,&ArbGenerator::updateFrequencyCallback);
    connect(arbGenWindow, &ArbGeneratorWindow::restartGenerating, this,&ArbGenerator::quickRestartCalback);
}

QWidget *ArbGenerator::getWidget()
{
    return arbGenWindow;
}

void ArbGenerator::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader==cmd->CONFIG){
        data.remove(0,4);
        moduleSpecification->parseSpecification(data);
        arbGenWindow->setSpecification(static_cast<ArbGeneratorSpec*>(moduleSpecification));
        showModuleControl();
    }else if(dataHeader==cmd->CMD_GEN_NEXT){
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
        arbGenWindow->setGeneratorRuning();
    }else if(dataHeader==cmd->CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH1){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setFrequencyLabels(0,(qreal)(freq)/signalLengths[0]);
    }else if(dataHeader==cmd->CMD_GEN_SIGNAL_REAL_SAMPLING_FREQ_CH2){
        quint32 freq;
        freq = qFromBigEndian<quint32>(data.right(4));
        arbGenWindow->setFrequencyLabels(1,(qreal)(freq)/signalLengths[1]);
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
    restartGenerator();
    setGeneratorDACMode();
    setModuleStatus(ModuleStatus::PAUSE);
}

void ArbGenerator::stopModule()
{
    stopGenerator();
    arbGenWindow->setGeneratorStopped();
}

void ArbGenerator::sendSignalCallback(){
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

    lengthToSend = signalLengths[0];
    lengthSent = memoryIndex = actualSend = 0;
    sendingChannel = 1;
    sendNextData();
}

void ArbGenerator::stopCallback()
{
    stopGenerator();
}

void ArbGenerator::updateFrequencyCallback() //this function counts only with two channels (logic for more channels as far more complex)
{
    if(arbGenWindow->getFrequency(1) != arbGenWindow->getFrequency(0) || signalLengths[0] != signalLengths[1]){
        //qDebug () << "updated independently";
        for (int i = 0;i<numChannelsUsed;i++){
            setSamplingFrequency(i,arbGenWindow->getFrequency(i)*signalLengths[i]);
        }

    }else{
        //qDebug () << "updated both synced";
        setSamplingFrequency(99,arbGenWindow->getFrequency(0)*signalLengths[0]);

    }
    genAskForFreq();
}

void ArbGenerator::quickRestartCalback()
{
    stopGenerator();
    startGenerator();
    qDebug () << "restarted";
}

void ArbGenerator::sendNextData()
{
    comm->write(cmd->GENERATOR+":"+cmd->CMD_GEN_DATA + " ");
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
    comm->write(cmd->GENERATOR,cmd->START);
}

void ArbGenerator::stopGenerator()
{
    setModuleStatus(ModuleStatus::PAUSE);
    comm->write(cmd->GENERATOR,cmd->CMD_GEN_STOP);
}

void ArbGenerator::restartGenerator()
{
    comm->write(cmd->GENERATOR,cmd->CMD_GEN_RESET);
}

void ArbGenerator::setGeneratorDACMode()
{
    comm->write(cmd->GENERATOR,cmd->CMD_GEN_MODE,cmd->CMD_MODE_DAC);
}

void ArbGenerator::setDataLength(int channel, int length)
{
    if(channel==0){
        comm->write(cmd->GENERATOR,cmd->DATA_LENGTH_CH1,length);
    }else if(channel==1){
        comm->write(cmd->GENERATOR,cmd->DATA_LENGTH_CH2,length);
    }else{
        qDebug () << "More channels not yet implemented";
    }
}

void ArbGenerator::setNumChannels(int numChannels)
{
    if(numChannels==1){
        comm->write(cmd->GENERATOR,cmd->CHANNELS,cmd->CHANNELS_1);
    }else if(numChannels==2){
        comm->write(cmd->GENERATOR,cmd->CHANNELS,cmd->CHANNELS_2);
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
    comm->write(cmd->GENERATOR,cmd->SAMPLING_FREQ,tmp);
}

void ArbGenerator::genAskForFreq()
{
    comm->write(cmd->GENERATOR,cmd->GET_REAL_SMP_FREQ);
}

void ArbGenerator::setOutputBuffer(bool isEnabled)
{
    if(isEnabled){
        comm->write(cmd->GENERATOR,cmd->CMD_GEN_OUTBUFF_ON);
    }else{
        comm->write(cmd->GENERATOR,cmd->CMD_GEN_OUTBUFF_OFF);
    }
}



