#ifndef PANELCURSORS_H
#define PANELCURSORS_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"

class PanelCursors : public QObject
{
    Q_OBJECT
public:
    explicit PanelCursors(QVBoxLayout *destination, QWidget *parent = nullptr);

signals:

private:
    WidgetButtons *channelButtons;

};

#endif // PANELCURSORS_H
