#ifndef COUNTER_H
#define COUNTER_H

#include <QDebug>
#include <QPointF>
#include <QtMath>

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

    /* High Frequency Counter */
    MovingAverage *movAvg;
    QString strQerr, strTerr, avgQerr;

    void parseHighFrequencyCounter(QByteArray metaData, QByteArray data);
    QString hfFormatRemainSec(uint countToGo, uint additionTime);
    void hfDisplayErrors();
    void displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr);
    void clearDisplay(WidgetDisplay *display);
    void specReceived();

private slots:
    void parseData(QByteArray);
    void writeConfiguration();

    void startModule();
    void stopModule();

    void switchCounterModeCallback(int index);
    void hfSwitchGateTimeCallback(int index);
    void hfSwitchQuantityCallback(int index);
    void hfSwitchErrorAvgCallback(int index);
    void hfDialAvgChangedCallback(float val);
};

#endif // COUNTER_H
