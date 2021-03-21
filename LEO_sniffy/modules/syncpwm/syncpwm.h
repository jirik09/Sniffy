#ifndef SYNCPWM_H
#define SYNCPWM_H

#include "syncpwmwindow.h"
#include "communication/commands.h"
#include "communication/comms.h"

#include "../../GUI/widgetcontrolmodule.h"

#include "../abstractmodule.h"

class SyncPwm : public AbstractModule
{
    Q_OBJECT

public:
    explicit SyncPwm(QObject *parent = nullptr);
    ~SyncPwm();

    QWidget* getWidget();

private:
    SyncPwmWindow *spwmWindow;
    SyncPwmConfig *config;

    void write(QByteArray feature, QByteArray param);
    void write(QByteArray feature, int param);

    void start();
    void stop();

    void enableChannel(int chanIndex, bool enable);
    void invertChannel(int chanIndex, bool invert);
    void setFreqChannel(int chanIndex, float val);
    void setDutyChannel(int chanIndex, float val);
    void setPhaseChannel(int chanIndex, float val);

signals:

private slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();

    void startModule();
    void stopModule();

    void buttonStartCallback(int index);
    void buttonEquidistantModeCallback(int index); // index not used

    void switchOnOffCh1Callback(int index); // index 0/1
    void switchOnOffCh2Callback(int index);
    void switchOnOffCh3Callback(int index);
    void switchOnOffCh4Callback(int index);

    void buttonInvertCh1Callback(int index); // index not used
    void buttonInvertCh2Callback(int index);
    void buttonInvertCh3Callback(int index);
    void buttonInvertCh4Callback(int index);

    void dialFreqCh1Callback(float val);
    void dialFreqCh2Callback(float val);
    void dialFreqCh3Callback(float val);
    void dialFreqCh4Callback(float val);

    void dialDutyCh1Callback(float val);
    void dialDutyCh2Callback(float val);
    void dialDutyCh3Callback(float val);
    void dialDutyCh4Callback(float val);

    void dialPhaseCh1Callback(float val);
    void dialPhaseCh2Callback(float val);
    void dialPhaseCh3Callback(float val);
    void dialPhaseCh4Callback(float val);
};

#endif // SYNCPWM_H
