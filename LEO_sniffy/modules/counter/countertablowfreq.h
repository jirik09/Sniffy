#ifndef COUNTERTABLOWFREQ_H
#define COUNTERTABLOWFREQ_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

#include "counterdefs.h"

class CounterTabLowFreq : public QObject
{
    Q_OBJECT
public:
    explicit CounterTabLowFreq(QVBoxLayout *destination, QWidget *parent = nullptr);

    WidgetDialRange *dialSampleCountCh1;
    WidgetDialRange *dialSampleCountCh2;
    WidgetButtons *buttonsChannelSwitch;
    WidgetButtons *buttonsQuantitySwitch;
    WidgetButtons *buttonsMultiplierSwitch;
    WidgetButtons *buttonsDutyCycleSwitch;

    const QString lfCh2BckgndColor = BCKGRND_COLOR_GREY;
    const QString lfCh2TextColor = TEXT_COLOR_GREY;

signals:

};

#endif // COUNTERTABLOWFREQ_H
