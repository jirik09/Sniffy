#ifndef COUNTER_H
#define COUNTER_H

#include <QDebug>
#include <QPointF>
#include <QtMath>

#include "counterwindow.h"
#include "communication/commands.h"
#include "communication/comms.h"
#include "math/movingaverage.h"
#include "../labelformator.h"

#include "../../GUI/widgetcontrolmodule.h"

#include "../abstractmodule.h"

class Counter : public AbstractModule
{
    Q_OBJECT

public:
    explicit Counter(QObject *parent = nullptr);
    ~Counter();

    Q_DISABLE_COPY(Counter)

    QWidget* getWidget();

    // Dispatch handlers (public solely so the static dispatch table in counter.cpp can take their addresses).
    // They are not intended for external module consumption; consider wrapping in a private
    // section plus a friend struct if broader encapsulation becomes important.
    void parseHighFrequencyCounter(QByteArray data);
    void parseLowFrequencyCounter(QByteArray data);
    void parseRatioCounter(QByteArray data);
    void parseIntervalsCounter(QByteArray data);

private:

    CounterWindow *cntWindow = nullptr; // QObject child (owned)
    CounterConfig *config = nullptr;     // QObject child (owned)

    void startCounting();
    void stopCounting();
    void buildModuleDescription(CounterSpec *spec);

    QString formatNumber(WidgetDisplay *display, double valToFormat, double error);
    QString formatErrNumber(WidgetDisplay *display, double errToFormat);
    void displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr);
    void specReceived();
    void write(QByteArray feature, QByteArray param);
    void write(QByteArray feature, int param);

    /* High Frequency Counter */
    MovingAverage *movAvg = nullptr; // QObject child
    QString strQerr, strTerr, avgQerr;
    float avg;    
    void hfReloadState();
    void hfDisplayErrors();    
    QString hfFormatRemainSec(uint countToGo, uint additionTime);

    /* Low Frequency Counter */
    void lfReloadState();
    void lfReloadStateQuantMeasurement();
    bool isRangeExceeded(double frequency);
    void lfSwitchQuantity(int actualLength, QByteArray channelQuantitiy);
    void lfSwitchMultiplier(int actualLength, QByteArray channelMultiplier);

    /* Ratio Counter */
    void ratReloadState();

    /* Intervals Counter */
    void intReloadState();

private slots:

    void parseData(QByteArray);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();

    void startModule();
    void stopModule();

    void showHoldButtonCallback();
    void holdCounter(bool held);

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

    /* Intervals meas. Counter */
    void intButtonsStartCallback();
    void intSwitchEventSequenceChangedCallback(int actualLength);
    void intEventAChangedCallback(int actualLength);
    void intEventBChangedCallback(int actualLength);
    void intDialTimeoutChangedCallback(float val);

    void discardHold(void);
};

//#define CALL_MEMBER_FUNCTION(object, ptrToMember)  ((object)->*(ptrToMember))

#endif // COUNTER_H
