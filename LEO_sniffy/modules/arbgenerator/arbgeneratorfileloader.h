#ifndef ARBGENERATORFILELOADER_H
#define ARBGENERATORFILELOADER_H

#include <QObject>
#include <QFile>
#include <QtMath>

#include "arbgeneratorspec.h"

class ArbGeneratorFileLoader : public QObject
{
    Q_OBJECT
public:
    explicit ArbGeneratorFileLoader(QObject *parent = nullptr);
    int parseFile(QString file);
    int getNumChannels();
    int getSignalLength(int channelIndex);
    QVector<qreal> getSignal(int channelIndex);
    qreal getSamplerateFrequency();
    bool isSamplerateDefined();
    void setSignalRangeParameters (int resolution, qreal rangeMin, qreal rangeMax);
    QString getInfoString();
    qreal getAmplitude(int channelIndex);
    qreal getOffset(int channelIndex);
    int getParsingErrors() const;

signals:

private:
    int numOfChannels = MAX_ARB_CHANNELS_NUM;
    QVector<QVector<qreal>> *data;
    qreal max[MAX_ARB_CHANNELS_NUM+1] = {ARBITRARY_DEFAULT_SIGNAL_MAX,ARBITRARY_DEFAULT_SIGNAL_MAX,ARBITRARY_DEFAULT_SIGNAL_MAX,ARBITRARY_DEFAULT_SIGNAL_MAX,ARBITRARY_DEFAULT_SIGNAL_MAX};  //coresponds to default arb data
    qreal min[MAX_ARB_CHANNELS_NUM+1] = {ARBITRARY_DEFAULT_SIGNAL_MIN,ARBITRARY_DEFAULT_SIGNAL_MIN,ARBITRARY_DEFAULT_SIGNAL_MIN,ARBITRARY_DEFAULT_SIGNAL_MIN,ARBITRARY_DEFAULT_SIGNAL_MIN};
    int resolution;
    qreal rangeMin;
    qreal rangeMax;
    qreal sampleRate;
    bool isSampleRateDefined = false;
    int parsingErrors = 0;
    QVector<qreal> defaultData;

    bool parse (QString in, qreal &output);
    void updateMinMax(qreal sample, int channelIndex);


};

#endif // ARBGENERATORFILELOADER_H
