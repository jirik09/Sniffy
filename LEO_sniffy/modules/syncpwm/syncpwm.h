#ifndef SYNCPWM_H
#define SYNCPWM_H

#include <QtEndian>

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

    void setDutyPhase(quint32 dutyCycle, quint32 phase, quint32 chanIndex);

    void write(QByteArray feature, QByteArray param);
    void write(QByteArray feature, int param);
    QByteArray intToSend(int param);

    void start();
    void stop();

    void stepGenEndNotif();

signals:

private slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();

    void startModule();
    void stopModule();

    void buttonStartCallback(int index);
    void switchStepModeCallback(int index);
    void buttonEquidistantModeCallback(int index); // index not used

    void switchOnOffCallback(int index, int chanIndex); // index 0/1
    void buttonInvertCallback(int index, int chanIndex); // index not used
    void dialFreqCallback(float val, int chanIndex);
    void dialDutyCallback(float val, int chanIndex);
    void dialPhaseCallback(float val, int chanIndex);

};

#endif // SYNCPWM_H
