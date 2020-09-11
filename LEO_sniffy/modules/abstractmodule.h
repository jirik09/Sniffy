#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H

#include <QObject>
#include "../GUI/moduledockwidget.h"
#include "../GUI/widgetcontrolmodule.h"
#include "../communication/comms.h"
#include "../communication/commands.h"

class AbstractModule : public QObject
{
    Q_OBJECT
public:
    explicit AbstractModule(QObject *parent = nullptr);

    virtual void parseData(QByteArray data) = 0;  //handling new data for the module (should not respond when not active)
    virtual void startModule() = 0; //module window has been opened - start the operation
    virtual void stopModule() = 0;  //module window has ben closed - stop it
    virtual void writeConfiguration() = 0; // configuration nas been loaded after startup (or default) - write it into device
    virtual QWidget* getWidget() = 0; //return pointer to widget - need to be able to dock it


    void setDockWidgetWindow(ModuleDockWidget *dockWidget);
    void setModuleControlWidget(WidgetControlModule *scpWidget);
    void setComms(Comms *communication);

    QByteArray getCommandPrefix();
    void hideModuleStatus();
    void setModuleStatus(ModuleStatus stat);

    void showModuleControl();
    void showModuleWindow();
    void closeModule();
    void disableModule();

    void setIcon (QString ImageURI);

    QString getModuleName();
    void setModuleName(QString value);

protected:
    QString moduleName;
    QString moduleIconURI;
    Commands *cmd;
    Comms *comm;
    QByteArray moduleCommandPrefix;

private:
    ModuleDockWidget *dockWidgetWindow;
    WidgetControlModule *moduleControlWidget;

public slots:
    void widgetControlClicked(ModuleStatus status);
};

#endif // ABSTRACTMODULE_H
