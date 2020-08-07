#ifndef COUNTER_H
#define COUNTER_H

#include <QDebug>
#include <QPointF>
#include <QtMath>

#include "counterwindow.h"
#include "counterconfig.h"
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

    QString formatHoldOn(uint countToGo, uint additionTime);
    void displayErrors(WidgetDisplay *display);
    void displayValues(WidgetDisplay *display, QString val, QString avg, QString qerr, QString terr);
    void clearDisplay(WidgetDisplay *display);

signals:
    void specReceived();

private slots:
    void parseData(QByteArray);
    void writeConfiguration();

    void startModule();
    void stopModule();

    void switchCounterModeCallback(int index);
    void switchGateTimeCallback(int index);
    void switchQuantityCallback(int index);
    void switchErrorAvgCallback(int index);
    void dialAvgChangedCallback(float val);
};

#endif // COUNTER_H
