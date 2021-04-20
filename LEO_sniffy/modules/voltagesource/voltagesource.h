#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include <QObject>
#include <QtEndian>

#include "../abstractmodule.h"
#include "voltagesourcespec.h"
#include "voltagesourceconfig.h"
#include "voltagesourcewindow.h"

class VoltageSource : public AbstractModule
{
    Q_OBJECT
public:
    explicit VoltageSource(QObject *parent = nullptr);
    QWidget* getWidget();

signals:


public slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();
    void startModule();
    void stopModule();

private slots:
    void voltageChangedCallback(qreal value, int channel);



private:
    VoltageSourceConfig *config;
    VoltageSourceSpec * spec;
    VoltageSourceWindow *voltSourceWindow;

    int DACData [MAX_VOLTAGE_SOURCE_CHANNELS] = {0,0,0,0};

    void sendDACVoltage();

};

#endif // VOLTAGESOURCE_H
