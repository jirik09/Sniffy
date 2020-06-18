#ifndef SCOPE_H
#define SCOPE_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>

#include "scopeconfig.h"
#include "scopespec.h"
#include "communication/Commands.h"

class Scope : public QObject
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);
    void initDefault();
    void setNumberOfChannels(int num);
    void sendCommandInt32(int number);
    void sendCommand(QByteArray);

    void stopScope();
    void startScope();
    void scopeNextData();

    float getTimebase();
    ScopeTriggerMode getTriggerMode();

    void channelEnableCallback(int buttonStatus);
    void triggerModeCallback(int buttonIndex);

signals:
    void scopeSpecified();
    void send(QByteArray);
    void scopeDataReceived(QVector<QVector<QPointF>>);

public slots:
    void parseData(QByteArray);


    void triggerEdgeCallback(int buttonIndex);
    void triggerChannelCallback(int index);

    void setTimebase(float div);
    void setTriggerLevel(float percentage);
    void setPretrigger(float percentage);

private:
    QVector<QVector<QPointF>> scopeData;

    Commands *cmd;
    ScopeConfig *config;
};

#endif // SCOPE_H
