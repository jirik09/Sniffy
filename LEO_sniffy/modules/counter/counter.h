#ifndef COUNTER_H
#define COUNTER_H

#include <QDebug>
#include <QPointF>
#include <QtMath>
#include <QEventLoop>
#include <QTimer>

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
    CounterConfig *config;
    CounterSpec *spec;
    QLocale loc;

    void startCounting();
    void stopCounting();

    QString formatNumber(double valToFormat, double error);

    /* Common functions */
    void displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr);
    void clearDisplay(WidgetDisplay *display);
    void specReceived();
    void msleep(int msec);

    /* High Frequency Counter */
    MovingAverage *movAvg;
    QString strQerr, strTerr, avgQerr;

    void parseHighFrequencyCounter(QByteArray data);
    QString hfFormatRemainSec(uint countToGo, uint additionTime);
    void hfDisplayErrors();

    /* Low Frequency Counter */
    void parseLowFrequencyCounter(QByteArray data);
    void lfSwitchQuantity(int index, QByteArray channelQuantitiy);
    void lfSwitchMultiplier(int index, QByteArray channelMultiplier);
    bool isRangeExceeded(double frequency);

private slots:

    void parseData(QByteArray);
    void writeConfiguration();

    void startModule();
    void stopModule();

    /* Common functions */
    void switchCounterModeCallback(int index);

    /* High Frequency Counter */
    void hfSwitchGateTimeCallback(int index);
    void hfSwitchQuantityCallback(int index);
    void hfSwitchErrorAvgCallback(int index);
    void hfDialAvgChangedCallback(float val);

    /* Low Frequency Counter */
    void lfSwitchChannelCallback(int index);
    void lfSwitchQuantityCallback(int index);
    void lfSwitchMultiplierCallback(int index);
    void lfSwitchDutyCycleCallback(int index);
    void lfDialSampleCountCh1ChangedCallback(float val);
    void lfDialSampleCountCh2ChangedCallback(float val);
};

#endif // COUNTER_H
