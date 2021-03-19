#ifndef SYNCPWM_H
#define SYNCPWM_H

#include "syncpwmwindow.h"
#include "communication/commands.h"
#include "communication/comms.h"

#include "../../GUI/widgetcontrolmodule.h"

#include "../abstractmodule.h"

class SyncPWM : public AbstractModule
{
    Q_OBJECT

public:
    explicit SyncPWM(QObject *parent = nullptr);
    ~SyncPWM();

    QWidget* getWidget();

private:
    SyncPWMWindow *spwmWindow;

    void write(QByteArray feature, QByteArray param);
    void write(QByteArray feature, int param);

signals:

private slots:
    void parseData(QByteArray data);
    void writeConfiguration();
    void parseConfiguration(QByteArray config);
    QByteArray getConfiguration();

    void startModule();
    void stopModule();
};

#endif // SYNCPWM_H
