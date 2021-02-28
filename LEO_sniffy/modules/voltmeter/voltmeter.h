#ifndef VOLTMETER_H
#define VOLTMETER_H

#include <QObject>

#include "../abstractmodule.h"
#include "voltmeterspec.h"
#include "voltmeterconfig.h"
#include "voltmeterwindow.h"
#include "channeldata.h"

#include "../scope/measurement.h"
#include "../scope/meascalculations.h"


class Voltmeter : public AbstractModule
{
    Q_OBJECT
public:
    explicit Voltmeter(QObject *parent = nullptr);
    QWidget* getWidget();


public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();

private slots:
    //In case hold is needed

    void showHoldButtonCallback();
    void holdButtonCallback(bool held);
    void updateMeasurement(QList<Measurement*> m);
    void setAveraging(int value);
    void setNumChannelsEnabled(int value);
    void resetMinMax();

private:
    VoltmeterConfig *config;
    VoltmeterWindow *voltWindow;

    double realVdd = 1;
    bool isReferenceMeasured = false;
    bool isStartup = true;
    int samplesTaken = 0;
    int samplesToTakeTotal = 4;
    int numChannelsEnabled = 1;

    QList<qreal> dataRawVoltage[MAX_VOLTMETER_CHANNELS];
    ChannelData data[MAX_VOLTMETER_CHANNELS];

    QList<Measurement *> voltMeasList;
    MeasCalculations *measCalc;

    QVector<QVector<QPointF>> *voltmeterData;

    void stopSampling();
    void startSampling();
    void restartSampling();
    void setNumberOfChannels(int num);
    void setVDDSampling();
    void setDefaultSampling();

    qreal getAverage(QList<qreal> *list);




};

#endif // VOLTMETER_H
