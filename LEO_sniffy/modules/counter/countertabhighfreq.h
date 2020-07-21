#ifndef COUNTERTABHIGHFREQ_H
#define COUNTERTABHIGHFREQ_H

#include <QObject>
#include <QVBoxLayout>
#include <QLabel>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"

class CounterTabHighFreq : public QObject
{
    Q_OBJECT
public:
    explicit CounterTabHighFreq(QVBoxLayout *destination, QWidget *parent = nullptr);

    WidgetDialRange *dialAveraging;
    WidgetButtons *buttonsQuantitySwitch;
    WidgetButtons *buttonsErrorSwitch;
    WidgetButtons *buttonsGateTime;
    QLabel *labelAutoInputPrescaler;

signals:    

private:

};

#endif // COUNTERTABHIGHFREQ_H
