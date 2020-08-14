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

    WidgetDialRange *dialSampleCount;
    WidgetButtons *buttonsChannelSwitch;
    WidgetButtons *buttonsQuantitySwitch;
    WidgetButtons *buttonsSampleCountMultiplierSwitch;
    WidgetButtons *buttonsDutyCycleSwitch;

signals:

};

#endif // COUNTERTABLOWFREQ_H
