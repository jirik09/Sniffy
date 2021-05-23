#ifndef SCOPE_H
#define SCOPE_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>
#include <QList>
#include <QSettings>

#include "timebaseandmemory.h"
#include "scopewindow.h"
#include "scopeconfig.h"
#include "scopespec.h"
#include "fftengine.h"


#include "communication/commands.h"
#include "communication/comms.h"
#include "measurement.h"
#include "meascalculations.h"
#include "mathcalculations.h"

#include "../abstractmodule.h"

class Scope : public AbstractModule
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);

    QWidget* getWidget();

public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();

    void startModule();
    void stopModule();

    void updateTimebase(float div);
    void updateCustomSamplingFreq(int freq);
    void updatePretrigger(float percentage);
    void updateTriggerLevel(float percentage);
    void updateTriggerMode(ScopeTriggerMode mode);
    void updateTriggerEdge(ScopeTriggerEdge edge);
    void updateTriggerChannel(int index);
    void updateMemoryPolicy(int memPolicyIndex);
    void updateMemoryCustomLength(int length);
    void updateResolution(int resolution);
    void updateChannelsEnable(int buttonStatus);
    void addMeasurement(Measurement *m);
    void updateMeasurement(QList<Measurement*> m);
    void updateMath(int errorPosition);
    void updateMathExpression(QString exp);
    void updateFFTConfig(int length, FFTWindow window, FFTType type, int channelIndex);
    void updateFFT();
    void clearMeasurement();

private:
    ScopeWindow *scpWindow;
    ScopeConfig *config;
    TimeBaseAndMemory *timeAndMemoryHandle;
    ScopeSpec *specification;
    MeasCalculations *measCalc;
    MathCalculations *mathCalc;
    FFTengine *FFTCalc;
    QVector<QVector<QPointF>> *scopeData;
    QString mathExpression;

    int FFTlength = 0;
    FFTWindow FFTwindow;
    FFTType FFTtype;
    int FFTChannelIndex = 0;

    bool isConfigurationWritten = false;

    void buildModuleDescription(ScopeSpec *spec);

    //private functions - writing into device only - no logic
    void stopSampling();
    void startSampling();
    void restartSampling();
    void setNumberOfChannels(int num);
    void setResolution(int res);
    void setTriggerChannel(int chan);
    void setTriggerMode(ScopeTriggerMode mode);
    void setTriggerEdge(ScopeTriggerEdge edge);
    void setSamplingFrequency(int samplingFreq);
    void setDataLength(int dataLength);

};

#endif // SCOPE_H
