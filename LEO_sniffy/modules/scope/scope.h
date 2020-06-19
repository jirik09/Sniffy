#ifndef SCOPE_H
#define SCOPE_H

#include <QObject>
#include <QDebug>
#include <QDataStream>
#include <QPointF>

#include "windowscope.h"

#include "scopeconfig.h"
#include "scopespec.h"
#include "communication/Commands.h"
#include "communication/comms.h"

#include "../../GUI/widgetmodule.h"
#include "../../GUI/moduledockwidget.h"

class Scope : public QObject
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);

    void setModuleWindow(WindowScope *scpWin);
    void setDockWidgetWindow(ModuleDockWidget *dockWidget);
    void setModuleControlWidget(WidgetModule *scpWidget);

    void setComms(Comms *communication);

    void writeConfiguration();

    void stopScope();
    void startScope();
    void scopeNextData();

    float getTimebase();
    ScopeTriggerMode getTriggerMode();
    void setNumberOfChannels(int num);
    void channelEnableCallback(int buttonStatus);
    void triggerModeCallback(int buttonIndex);

signals:
    void scopeSpecificationLoaded();
    void scopeDataReceived(QVector<QVector<QPointF>>);

public slots:
    void closeModule();

private slots:
    void parseData(QByteArray);

    void widgetClicked(ModuleStatus status);

    void triggerEdgeCallback(int buttonIndex);
    void triggerChannelCallback(int index);

    void setTimebase(float div);
    void setTriggerLevel(float percentage);
    void setPretrigger(float percentage);

private:
    QVector<QVector<QPointF>> scopeData;

    Commands *cmd;
    Comms *comm;

    ScopeConfig *config;

    WindowScope *scpWindow;
    ModuleDockWidget *scpDockWidgetWindow;
    WidgetModule *scpModuleControlWidget;
};

#endif // SCOPE_H
