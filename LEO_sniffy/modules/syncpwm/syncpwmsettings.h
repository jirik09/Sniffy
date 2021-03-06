#ifndef SYNCPWMSETTINGS_H
#define SYNCPWMSETTINGS_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

#include "syncpwmspec.h"
#include "syncpwmconfig.h"
#include "syncpwmdefs.h"

class SyncPwmSettings : public QObject
{
    Q_OBJECT
public:
    explicit SyncPwmSettings(QVBoxLayout *destination, SyncPwmConfig *config, QWidget *parent = nullptr);

    WidgetSeparator *separator[CHANNELS_NUM];    

    WidgetButtons *buttonStart;
    WidgetButtons *buttonEquidist;
    WidgetSwitch *switchStepMode;

    WidgetSwitch *onOffCh[CHANNELS_NUM];
    WidgetButtons *inverCh[CHANNELS_NUM];

    WidgetDialRange *dialFreqCh[CHANNELS_NUM];
    WidgetDialRange *dialDutyCh[CHANNELS_NUM];
    WidgetDialRange *dialPhaseCh[CHANNELS_NUM];

    void setSpecification(SyncPwmSpec *spec);

    void configControlElements(QWidget *parent, int i, int phase);
    void greyOutComplementChanFreqDials(int chanIndex);
    void setRealFrequency(double val, int chanIndex);    

private:
    QLocale loc;
    SyncPwmConfig *config;
    SyncPwmSpec *spec;

    QString formatNumber(double val);

signals:

};

#endif // SYNCPWMSETTINGS_H
