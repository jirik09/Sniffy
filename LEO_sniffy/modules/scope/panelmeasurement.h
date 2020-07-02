#ifndef PANELMEASUREMENT_H
#define PANELMEASUREMENT_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"

#include"measurement.h"

class PanelMeasurement: public QObject
{
    Q_OBJECT
public:
    explicit PanelMeasurement(QVBoxLayout *destination, QWidget *parent = nullptr);

signals:
    void measurementAdded(Measurement *m);
    void measurementClearAll();

private slots:
    void RMSClicked();
    void meanClicked();
    void minClicked();
    void maxClicked();
    void pkpkClicked();

    void freqClicked();
    void periodClicked();
    void dutyClicked();
    void highClicked();
    void lowClicked();
    void phaseClicked();
    void clearClicked();

    void setMeasButtonsColor(int index);

private:
    WidgetButtons *channelButtons;
    WidgetButtons *channelButtonPhaseA;
    WidgetButtons *channelButtonPhaseB;

    QList<WidgetButtons*> measButtons;
};

#endif // PANELMEASUREMENT_H
