#ifndef GENCOMMONS_H
#define GENCOMMONS_H

#include <QObject>
#include <QDebug>
#include <QtMath>
#include <QtEndian>

#include "../../communication/commands.h"
#include "../../communication/comms.h"
#include "arbgeneratordefs.h"


class GenCommons : public QObject
{
    Q_OBJECT
public:
    explicit GenCommons(QByteArray prefix,Comms *comm,int maxSamplingRate,QObject *parent = nullptr);

    void startGenerator();
    void stopGenerator();
    void setGeneratorDACMode();
    void setGeneratorPWMMode();
    void generatorDeinit();
    void setDataLength(int channel, int length);
    void setNumChannels(int numChannels);
    void setSamplingFrequency (int channel, qreal freq);
    void setPWMFrequency (int channel, qreal freq);
    void genAskForFreq();
    void setOutputBuffer (bool isEnabled);
    int getSignaLength(int channelIndex);

    //data sending functions
    void setSignaltoSend(QList<QList<int>> *data);
    void sendNext();
    bool isSentAll();
    qreal getProgress();


private:
    QByteArray prefix;
    int maxSmplFreq;
    Comms *comm;
    Commands *cmd;

    //data sending
    QList<QList<int>> generatorData;
    bool allSent = false;
    int lengthToSend;
    int lengthSent;
    int memoryIndex;
    int actualSend;
    int sendingChannel;
    int numChannelsUsed;
    int totalToSend;
    int totalSent;
    int signalLengths[MAX_ARB_CHANNELS_NUM] = {0};
    void sendNextBlock();

signals:

};

#endif // GENCOMMONS_H
