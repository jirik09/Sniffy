#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>

#include "windowcounter.h"

#include "counterconfig.h"
#include "counterspec.h"
#include "communication/Commands.h"
#include "communication/comms.h"

#include "../../GUI/widgetmodule.h"
#include "../../GUI/moduledockwidget.h"

class Counter : public QObject
{
    Q_OBJECT
public:
    explicit Counter(QObject *parent = nullptr);

    void setModuleWindow(WindowCounter *cntWin);
    void setDockWidgetWindow(ModuleDockWidget *dockWidget);
    void setModuleControlWidget(WidgetModule *cntWidget);

    void setComms(Comms *communication);

signals:

private:
    Commands *cmd;
    Comms *comm;

    CounterConfig *config;

    WindowCounter *cntWindow;
    ModuleDockWidget *cntDockWidgetWindow;
    WidgetModule *cntModuleControlWidget;

};

#endif // COUNTER_H
