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

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/moduledockwidget.h"

#include "../abstractmodule.h"

class Scope : public AbstractModule
{
    Q_OBJECT
public:
    explicit Scope(QObject *parent = nullptr);

    void setModuleWindow(WindowScope *scpWin);
  //  void setDockWidgetWindow(ModuleDockWidget *dockWidget);
 //   void setModuleControlWidget(WidgetControlModule *scpWidget);

 //   void setComms(Comms *communication);

    void writeConfiguration();

    void stopModule();
    void startModule();
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
  //  void closeModule();

private slots:
    void parseData(QByteArray);

    void triggerEdgeCallback(int buttonIndex);
    void triggerChannelCallback(int index);

    void setTimebase(float div);
    void setTriggerLevel(float percentage);
    void setPretrigger(float percentage);

private:
    QVector<QVector<QPointF>> scopeData;

    ScopeConfig *config;

    WindowScope *scpWindow;
};

#endif // SCOPE_H
