#ifndef PANELCURSORS_H
#define PANELCURSORS_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"

class PanelCursors : public QObject
{
    Q_OBJECT
public:
    explicit PanelCursors(QVBoxLayout *destination, QWidget *parent = nullptr);
    void ValidatePanelGUI(int type);

signals:

public:
    WidgetButtons *channelButtons;
    WidgetButtons *cursorTypeButtons;
    WidgetDialRange *cursorHorADial;
    WidgetDialRange *cursorHorBDial;

    WidgetDialRange *cursorVerADial;
    WidgetDialRange *cursorVerBDial;

    WidgetDialRange *cursorFFTHorADial;
    WidgetDialRange *cursorFFTHorBDial;

    WidgetDialRange *cursorFFTVerADial;
    WidgetDialRange *cursorFFTVerBDial;


};

#endif // PANELCURSORS_H
