#ifndef SYNCPWMSETTINGS_H
#define SYNCPWMSETTINGS_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

#include "syncpwmconfig.h"
#include "syncpwmdefs.h"

class SyncPwmSettings : public QObject
{
    Q_OBJECT
public:
    explicit SyncPwmSettings(QVBoxLayout *destination, SyncPwmConfig *config, QWidget *parent = nullptr);

    WidgetButtons *buttonStart;
    WidgetButtons *buttonEquidist;
    WidgetSwitch *switchStepMode;

    WidgetSwitch *onOffCh[CHANNELS_NUM];
    WidgetButtons *inverCh[CHANNELS_NUM];

    WidgetDialRange *dialFreqCh[CHANNELS_NUM];
    WidgetDialRange *dialDutyCh[CHANNELS_NUM];
    WidgetDialRange *dialPhaseCh[CHANNELS_NUM];

    const QString chanColor[CHANNELS_NUM] =  {COLOR_BLUE,
                                              COLOR_GREEN,
                                              COLOR_ORANGE,
                                              COLOR_PURPLE};

    void greyOutComplementChanFreqDials(int chanIndex);
    void setFreqDial(float val, int chanIndex);
    void setPhaseDial(float val, int chanIndex);
    void setDutyDial(float val, int chanIndex);

private:

    SyncPwmConfig *config;

signals:

};

#endif // SYNCPWMSETTINGS_H
