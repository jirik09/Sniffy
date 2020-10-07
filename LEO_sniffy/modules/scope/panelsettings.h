#ifndef PANELSETTINGS_H
#define PANELSETTINGS_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"

class PanelSettings : public QObject
{
    Q_OBJECT
public:
    explicit PanelSettings(QVBoxLayout *destination, QWidget *parent = nullptr);

signals:

public:
    WidgetDial *dialTimeBase;
    WidgetButtons *buttonsChannelEnable;
    WidgetButtons *buttonsTriggerMode;
    WidgetButtons *buttonsTriggerChannel;
    WidgetButtons *buttonsTriggerEdge;

    WidgetDialRange *dialPretrigger;
    WidgetDialRange *dialTriggerValue;

    WidgetButtons *buttonsMemorySet;

    WidgetButtons *buttonsChannelVertical;
    WidgetDial *dialVerticalScale;
    WidgetDialRange *dialVerticalShift;

private:
    void fillTimeBase();

    const QString verticalControlColor = "color:"+Colors::getChannelColorString(0);
    const QString verticalControlBcgrColor = "background-color:"+Colors::getChannelColorString(0);

};

#endif // PANELSETTINGS_H
