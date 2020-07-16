#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>

#include "counterwindow.h"
#include "counterconfig.h"
#include "counterspec.h"

#include "counterconfig.h"
#include "counterspec.h"
#include "communication/commands.h"
#include "communication/comms.h"

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

signals:

private slots:
    void parseData(QByteArray);
    void writeConfiguration();

    void startModule();
    void stopModule();

    void switchCounterModeCallback(int index);
    void switchGateTimeCallback(int index);

private:
    CounterWindow *cntWindow;
    CounterConfig *config;
    CounterSpec *specification;
    QVector<QVector<QPointF>> *cntData;

    void startCounting();
    void stopCounting();
};

#endif // COUNTER_H
