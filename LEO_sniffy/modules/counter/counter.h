#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>
#include <QtMath>

#include "counterwindow.h"
#include "counterconfig.h"
#include "counterspec.h"
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
    CounterSpec *specification;

    MovingAverage *movAvg;

    void startCounting();
    void stopCounting();

    /* High Frequency Counter */
    bool isFrequency();

signals:

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
