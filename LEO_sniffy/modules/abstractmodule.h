#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H

#include <QObject>
#include "../GUI/moduledockwidget.h"
#include "../GUI/widgetcontrolmodule.h"
#include "../communication/comms.h"
#include "../communication/Commands.h"

class AbstractModule : public QObject
{
    Q_OBJECT
public:
    explicit AbstractModule(QObject *parent = nullptr);
    //virtual setModuleWindow()  // Module window probably cannot be abstract

    virtual void parseData(QByteArray data) = 0;
    virtual void startModule() = 0;
    virtual void stopModule() = 0;
    virtual void writeConfiguration() = 0;
    virtual QWidget* getWidget() = 0;


    ModuleDockWidget *dockWidgetWindow;
    WidgetControlModule *moduleControlWidget;
    QByteArray moduleCommandPrefix;
    QString moduleName;
    Commands *cmd;
    Comms *comm;

public slots:

    void widgetControlClicked(ModuleStatus status);
    void setDockWidgetWindow(ModuleDockWidget *dockWidget);
    void setModuleControlWidget(WidgetControlModule *scpWidget);
    void setCommandPrefix(QByteArray prefix);

    void setComms(Comms *communication);

    void closeModule();

};

#endif // ABSTRACTMODULE_H
