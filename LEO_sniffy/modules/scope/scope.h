#ifndef SCOPE_H
#define SCOPE_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>

#include "windowscope.h"

#include "scopeconfig.h"
#include "scopespec.h"
#include "communication/Commands.h"
#include "communication/comms.h"

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/moduledockwidget.h"

#include "../abstractmodule.h"

class Scope : public AbstractModule
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);

    QWidget* getWidget();

signals:
    //module will probably emit signal to close other resources
    //(must be done in abstract module and handled in device.cpp)

public slots:
    void parseData(QByteArray);
    void writeConfiguration();

    void startModule();
    void stopModule();


    void updateTriggerEdge(ScopeTriggerEdge edge);
    void updateTriggerChannel(int index);
    void updateChannelsEnable(int buttonStatus);
    void updateTriggerMode(ScopeTriggerMode mode);

    void updateTimebase(float div);
    void updateTriggerLevel(float percentage);
    void updatePretrigger(float percentage);

private:
    WindowScope *scpWindow;
    ScopeConfig *config;
    QVector<QVector<QPointF>> scopeData;


    //private functions - writing into device only - no logic
    void stopSampling();
    void startSampling();
    void restartSampling();
    void setNumberOfChannels(int num);
    void setTriggerChannel(int chan);
    void setTriggerMode(ScopeTriggerMode mode);
    void setTriggerEdge(ScopeTriggerEdge edge);
    void setSamplingFrequency(int samplingFreq);

};

#endif // SCOPE_H
