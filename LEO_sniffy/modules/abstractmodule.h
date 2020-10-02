#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H

#include <QObject>
#include <QSettings>
#include "../GUI/moduledockwidget.h"
#include "../GUI/widgetcontrolmodule.h"
#include "../communication/comms.h"
#include "../communication/commands.h"

#include "../GUI/widgetbuttons.h"
#include "../GUI/widgetdial.h"
#include "../GUI/widgetdialrange.h"
#include "../GUI/widgetswitch.h"
#include "../GUI/widgettab.h"

class AbstractModule : public QObject
{
    Q_OBJECT
public:
    explicit AbstractModule(QObject *parent = nullptr);

    virtual void parseData(QByteArray data) = 0;  //handling new data for the module (should not respond when not active)
    virtual void startModule() = 0; //module window has been opened - start the operation
    virtual void stopModule() = 0;  //module window has ben closed - stop it
    virtual void writeConfiguration() = 0; // configuration nas been loaded after startup (or default) - write it into device
    virtual void parseConfiguration(QByteArray config) = 0;
    virtual QByteArray getConfiguration() = 0;
    virtual QWidget* getWidget() = 0; //return pointer to widget - need to be able to dock it

    void saveGeometry(QSettings &layout);
    void restoreGeometry(QSettings &layout);


    void setDockWidgetWindow(ModuleDockWidget *dockWidget);
    void setModuleControlWidget(WidgetControlModule *scpWidget);
    void setComms(Comms *communication);

    QByteArray getCommandPrefix();
    void hideModuleStatus();
    void setModuleStatus(ModuleStatus stat);
    ModuleStatus getModuleStatus();

    void showModuleControl();
    void showModuleWindow();
    void closeModule();
    void disableModule();

    void setIcon (QString ImageURI);

    QString getModuleName();
    void setModuleName(QString value);

    void showModuleHoldButton();

protected:
    QString moduleName;
    QString moduleIconURI;
    Commands *cmd;
    Comms *comm;
    QByteArray moduleCommandPrefix;

private:
    ModuleDockWidget *dockWidgetWindow;
    WidgetControlModule *moduleControlWidget;
    bool dockWinCreated = false;

public slots:
    void widgetControlClicked(ModuleStatus status);
    void held(bool held);

signals:
    void moduleCreated();
    void holdClicked(bool held);

};

#endif // ABSTRACTMODULE_H
