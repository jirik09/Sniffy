#ifndef COUNTERTABRATIO_H
#define COUNTERTABRATIO_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"

#include "counterdefs.h"

class CounterTabRatio : public QObject
{
    Q_OBJECT
public:
    explicit CounterTabRatio(QVBoxLayout *destination, QWidget *parent = nullptr);

    WidgetDialRange *dialSampleCount;
    WidgetButtons *buttonRetrigger;

signals:

};

#endif // COUNTERTABRATIO_H
