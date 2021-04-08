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

    const QString PATH_IMG_EDGE_RISING = Colors::getGraphicsPath()+"cnt_edge_rising.png";
    const QString PATH_IMG_EDGE_FALLING = Colors::getGraphicsPath()+"cnt_edge_falling.png";
    const QString PATH_IMG_EVENT_AB_RR = Colors::getGraphicsPath()+"cnt_ab_rr.png";
    const QString PATH_IMG_EVENT_AB_RF = Colors::getGraphicsPath()+"cnt_ab_rf.png";
    const QString PATH_IMG_EVENT_AB_FR = Colors::getGraphicsPath()+"cnt_ab_fr.png";
    const QString PATH_IMG_EVENT_AB_FF = Colors::getGraphicsPath()+"cnt_ab_ff.png";
    const QString PATH_IMG_EVENT_BA_RR = Colors::getGraphicsPath()+"cnt_ba_rr.png";
    const QString PATH_IMG_EVENT_BA_RF = Colors::getGraphicsPath()+"cnt_ba_rf.png";
    const QString PATH_IMG_EVENT_BA_FR = Colors::getGraphicsPath()+"cnt_ba_fr.png";
    const QString PATH_IMG_EVENT_BA_FF = Colors::getGraphicsPath()+"cnt_ba_ff.png";

    void setSeqPicture();

private slots:    
    void eventsSequenceChangedCallback(int index);
    void eventAEdgeChangedCallback(int index);
    void eventBEdgeChangedCallback(int index);
};

#endif // COUNTERTABINTERVALS_H
