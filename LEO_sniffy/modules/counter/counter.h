#ifndef COUNTER_H
#define COUNTER_H

#include <QDebug>
#include <QPointF>
#include <QtMath>
#include <QEventLoop>

#include "counterwindow.h"
#include "communication/commands.h"
#include "communication/comms.h"
#include "math/movingaverage.h"

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/moduledockwidget.h"

#include "../abstractmodule.h"

class Counter : public AbstractModule
{
    Q_OBJECT

public:
    explicit Counter(QObject *parent = nullptr);
    ~Counter();

    QWidget* getWidget();

private:

    CounterWindow *cntWindow;
    CounterConfig *conf;
    CounterSpec *spec;
    QLocale loc;

    void startCounting();
    void stopCounting();

    /* Common functions */
    //typedef void (Counter::*funPointer)();
//    /*funPointer*/ void (Counter::*reloadModeState[4])() = { &Counter::hfReloadState,
//                                          &Counter::lfReloadState,
//                                          &Counter::ratReloadState,
//                                          &Counter::intReloadState };

    QString formatNumber(double valToFormat, double error);
    QString formatErrNumber(double errToFormat);
    void displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr);
    void specReceived();
    void write(QByteArray feature, QByteArray param);
    void write(QByteArray feature, int param);

    /* High Frequency Counter */
    MovingAverage *movAvg;
    QString strQerr, strTerr, avgQerr;

    QVector<QPointF> *historyData;
    int histDataLength = 1000;
    float rememberMax = 0;
    qreal timeAxisMax = 0, timeAxisMin = 0;

    void parseHighFrequencyCounter(QByteArray data);
    void hfReloadState();
    void hfDisplayErrors();    
    QString hfFormatRemainSec(uint countToGo, uint additionTime);

    /* Low Frequency Counter */
    void parseLowFrequencyCounter(QByteArray data);
    void lfReloadState();
    void lfReloadStateQuantMeasurement();
    bool isRangeExceeded(double frequency);
    void lfSwitchQuantity(int actualLength, QByteArray channelQuantitiy);
    void lfSwitchMultiplier(int actualLength, QByteArray channelMultiplier);

    /* Ratio Counter */
    void parseRatioCounter(QByteArray data);
    void ratReloadState();

    /* Intervals Counter */
//    bool seqAB = false, eventA = false, eventB = false;

    void parseIntervalsCounter(QByteArray data);
    void intReloadState();

private slots:

    void parseData(QByteArray);
    void writeConfiguration();

    void startModule();
    void stopModule();

    /* Common functions */
    void switchCounterModeCallback(int actualLength);

    /* High Frequency Counter */
    void hfSwitchGateTimeCallback(int actualLength);
    void hfSwitchQuantityCallback(int actualLength);
    void hfSwitchErrorAvgCallback(int actualLength);
    void hfDialAvgChangedCallback(float val);

    /* Low Frequency Counter */
    void lfSwitchChannelCallback(int actualLength);
    void lfSwitchQuantityCallback(int actualLength);
    void lfSwitchMultiplierCallback(int actualLength);
    void lfSwitchDutyCycleCallback(int actualLength);
    void lfDialSampleCountCh1ChangedCallback(float val);
    void lfDialSampleCountCh2ChangedCallback(float val);

    /* Ratio Counter */
    void ratDialSampleCountChangedCallback(float val);
    void ratRetriggerCallback(int actualLength);

    /* Intervals meas. Counter */
    void intButtonsStartCallback();
    void intSwitchEventSequenceChangedCallback(int actualLength);
    void intEventAChangedCallback(int actualLength);
    void intEventBChangedCallback(int actualLength);
    void intDialTimeoutChangedCallback(float val);
};

//#define CALL_MEMBER_FUNCTION(object, ptrToMember)  ((object)->*(ptrToMember))

#endif // COUNTER_H
