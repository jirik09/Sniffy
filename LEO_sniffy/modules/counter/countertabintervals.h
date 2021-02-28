#ifndef COUNTERTABINTERVALS_H
#define COUNTERTABINTERVALS_H

#include <QObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetswitch.h"

#include "counterdefs.h"

class CounterTabIntervals : public QObject
{
    Q_OBJECT
public:
    explicit CounterTabIntervals(QVBoxLayout *destination, QWidget *parent = nullptr);

    WidgetButtons *buttonsStart;
    WidgetButtons *buttonsEventsSeq;

    WidgetSwitch *switchEdgeEventA;
    WidgetSwitch *switchEdgeEventB;

    WidgetDialRange *dialTimeout;

    QHBoxLayout *horizontalLayout_switchArea;

    QLabel *labelPicEventsSeq;

    QPixmap *pixmapEventsSeq;    

private:
    bool seqAB = false, eventA = false, eventB = false;
    int width, height;

    void setSeqPicture();

private slots:    
    void eventsSequenceChangedCallback(int index);
    void eventAEdgeChangedCallback(int index);
    void eventBEdgeChangedCallback(int index);
};

#endif // COUNTERTABINTERVALS_H
