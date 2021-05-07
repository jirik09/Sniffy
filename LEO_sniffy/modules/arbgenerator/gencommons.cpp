#include "gencommons.h"

GenCommons::GenCommons(QByteArray prefix,Comms *comm, int maxSamplingRate, QObject *parent) : QObject(parent)
{
    if(prefix.length()!=4){
        qDebug () << "FATAL ERROR module prefix has to be 4 chars";
    }else{
        this->prefix = prefix;
        this->comm = comm;
        maxSmplFreq = maxSamplingRate;
    }
}

void GenCommons::startGenerator()
{
    comm->write(prefix,cmd->START);
}

void GenCommons::stopGenerator()
{
    comm->write(prefix,cmd->CMD_GEN_STOP);
}

void GenCommons::setGeneratorDACMode()
{
    comm->write(prefix,cmd->CMD_GEN_MODE,cmd->CMD_MODE_DAC);
}

void GenCommons::setGeneratorPWMMode()
{
    comm->write(prefix,cmd->CMD_GEN_MODE,cmd->CMD_MODE_PWM);
}

void GenCommons::generatorDeinit()
{
    comm->write(prefix,cmd->DEINIT);
}

void GenCommons::setDataLength(int channel, int length)
{
    if(channel==0){
        comm->write(prefix,cmd->DATA_LENGTH_CH1,length);
    }else if(channel==1){
        comm->write(prefix,cmd->DATA_LENGTH_CH2,length);
    }else{
        qDebug () << "More channels not yet implemented";
    }
}

void GenCommons::setNumChannels(int numChannels)
{
    if(numChannels==1){
        comm->write(prefix,cmd->CHANNELS,cmd->CHANNELS_1);
    }else if(numChannels==2){
        comm->write(prefix,cmd->CHANNELS,cmd->CHANNELS_2);
    }else{
        qDebug () << "More channels not yet implemented";
    }
}

void GenCommons::setSamplingFrequency(int channel, qreal freq)
{
    int tmp = (int)(round(freq))*256 + (channel+1)%256;

    if(round(freq) > maxSmplFreq){
        tmp = maxSmplFreq*256 + (channel+1)%256;
    }
    comm->write(prefix,cmd->SAMPLING_FREQ,tmp);
}

void GenCommons::setPWMFrequency(int channel, qreal freq)
{
    if(channel==0){
        comm->write(prefix,cmd->CMD_GEN_PWM_FREQ_CH1,freq);
    }else{
        comm->write(prefix,cmd->CMD_GEN_PWM_FREQ_CH2,freq);
    }
}

void GenCommons::genAskForFreq()
{
    comm->write(prefix,cmd->GET_REAL_SMP_FREQ);
}

void GenCommons::setOutputBuffer(bool isEnabled)
{
    if(isEnabled){
        comm->write(prefix,cmd->CMD_GEN_OUTBUFF_ON);
    }else{
        comm->write(prefix,cmd->CMD_GEN_OUTBUFF_OFF);
    }
}

int GenCommons::getSignaLength(int channelIndex)
{
    if(channelIndex<numChannelsUsed)
        return generatorData[channelIndex].length();
    return -1;
}

/* Multiple channels */
void GenCommons::setSignalToSend(QList<QList<int>> *data)
{
    generatorData = *data;
    numChannelsUsed = generatorData.length();
    totalToSend = totalSent = 0;
    allSent = false;

    //Memory demand (#Chan * length) has to be reduced first
    if (numChannelsUsed > 1) {
        for(int i = 0;i<numChannelsUsed;i++){
            signalLengths[i] = generatorData[i].length();
            setDataLength(i, signalLengths[i]);
            totalToSend +=  signalLengths[i];
        }
        setNumChannels(numChannelsUsed);
    }else{
        setNumChannels(numChannelsUsed);
        signalLengths[0] = generatorData[0].length();
        setDataLength(0,signalLengths[0]);
        totalToSend =  signalLengths[0];
    }

    lengthToSend = signalLengths[0];
    lengthSent = memoryIndex = actualSend = 0;
    sendingChannel = 1;
}

/* Single channel */
void GenCommons::setSignalToSend(QList<quint8> *data)
{
    for(int i = 0; i < data->length(); i++)
        generatorData[0][i] = data->at(i);

    numChannelsUsed = 1;
    totalToSend = totalSent = 0;
    allSent = false;

    setNumChannels(1);
    signalLengths[0] = generatorData[0].length();
    setDataLength(0, signalLengths[0]);
    totalToSend =  signalLengths[0];

    lengthToSend = signalLengths[0];
    lengthSent = memoryIndex = actualSend = 0;
    sendingChannel = 1;
}

void GenCommons::sendNext()
{
    if (lengthToSend == 0){
        if (sendingChannel == 1 && numChannelsUsed == 2) {
            lengthToSend = generatorData[1].length();
            lengthSent = 0;
            memoryIndex = 0;
            actualSend = 0;
            sendingChannel = 2;
            sendNextBlock();
        }else if (sendingChannel == numChannelsUsed) {
            allSent = true;
        }
    }else {
        sendNextBlock();
    }
}

bool GenCommons::isSentAll()
{
    return allSent;
}

qreal GenCommons::getProgress()
{
    return (qreal)totalSent*100.0/totalToSend;
}

void GenCommons::sendNextBlock()
{
    comm->write(prefix+":"+cmd->CMD_GEN_DATA + " ");
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
        qint16  sample = qFromBigEndian<qint16>(generatorData[sendingChannel-1][lengthSent+i]);
        dataStreamData << sample;
    }
    comm->write(tmpData+";");
    lengthSent += actualSend;
    lengthToSend -= actualSend;
    memoryIndex += actualSend;
    totalSent += actualSend;
}
