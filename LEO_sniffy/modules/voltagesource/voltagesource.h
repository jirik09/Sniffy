#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include <QObject>

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
    //In case hold is needed



private:
    VoltageSourceConfig *config;
    VoltageSourceWindow *voltSourceWindow;

    void sendDACVoltage(int channelIndex,int dacValue);

};

#endif // VOLTAGESOURCE_H
