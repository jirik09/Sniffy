#ifndef ARBGENERATOR_H
#define ARBGENERATOR_H

#include <QObject>
#include <QtEndian>

#include "../abstractmodule.h"
#include "arbgeneratorspec.h"
#include "arbgeneratorconfig.h"
#include "arbgeneratorwindow.h"
#include "arbgeneratordefs.h"


class ArbGenerator : public AbstractModule
{
    Q_OBJECT
public:
    explicit ArbGenerator(QObject *parent = nullptr);
    QWidget* getWidget();

signals:


public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();


private slots:
    void sendSignalCallback();
    void stopCallback();
    void updateFrequencyCallback();
    void quickRestartCalback();

private:
    ArbGeneratorConfig *config;
    ArbGeneratorWindow *arbGenWindow;

    int lengthToSend;
    int lengthSent;
    int memoryIndex;
    int actualSend;
    int sendingChannel;
    int numChannelsUsed;
    int totalToSend;
    int totalSent;
    int signalLengths[MAX_ARB_CHANNELS_NUM] = {0};
    QList<QList<int>> GeneratorData;

    void startGenerator ();
    void stopGenerator ();
    void restartGenerator();
    void setGeneratorDACMode();
    void setDataLength(int channel, int length);
    void setNumChannels(int numChannels);
    void setSamplingFrequency (int channel, qreal freq);
    void genAskForFreq();
    void setOutputBuffer (bool isEnabled);
    void sendNextData();


};

#endif // ARBGENERATOR_H
