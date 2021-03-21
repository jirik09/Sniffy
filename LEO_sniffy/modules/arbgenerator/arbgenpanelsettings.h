#ifndef ARBGENPANELSETTINGS_H
#define ARBGENPANELSETTINGS_H

#include <QObject>
#include <QVBoxLayout>
#include <QScrollArea>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgettextinput.h"

#include "arbgeneratordefs.h"
#include "arbgeneratorconfig.h"

class ArbGenPanelSettings : public QObject
{
    Q_OBJECT
public:
    explicit ArbGenPanelSettings(QVBoxLayout *destination, ArbGeneratorConfig *config, QWidget *parent = nullptr);

signals:

public:

    //common
    WidgetButtons *buttonsEnable;
    WidgetButtons *buttonsMemory;
    WidgetTextInput *customLengthInput;

    //for each channel
    WidgetButtons *buttonsShape[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialFreqCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialOffsetCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialAmplitudeCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialDutyCh[MAX_ARB_CHANNELS_NUM];
    WidgetDialRange *dialPhaseCh[MAX_ARB_CHANNELS_NUM];
    WidgetSwitch *syncwithCh1[MAX_ARB_CHANNELS_NUM];

private:

    ArbGeneratorConfig *config;

};

#endif // ARBGENPANELSETTINGS_H
