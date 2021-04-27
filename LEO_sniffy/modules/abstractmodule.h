#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QFile>

#include "../GUI/moduledockwidget.h"
#include "../GUI/widgetcontrolmodule.h"
#include "../communication/comms.h"
#include "../communication/commands.h"
#include "../graphics/abstracttheme.h"
#include "../graphics/themes/dark.h"
#include "../graphics/themes/light.h"

#include "../GUI/widgetbuttons.h"
#include "../GUI/widgetdial.h"
#include "../GUI/widgetdialrange.h"
#include "../GUI/widgetswitch.h"
#include "../GUI/widgettab.h"
#include "../GUI/widgetselection.h"
#include "../GUI/widgettextinput.h"

#include "abstractspecification.h"

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
    void showModuleDescription(QString name, QList<QString> labels, QList<QString> values);
    void showModuleWindow();
    void closeModule();
    void disableModule();
    bool isActive();

    void setIcon (QString ImageURI);

    QString getModuleName();
    void setModuleName(QString value);

    void showModuleHoldButton(bool show);
    void enableModuleHoldButton(bool enable);

    int getResources();

    bool isModuleRestored() const;
    void setModuleRestored(bool value);

protected:
    QString moduleName;
    QString moduleIconURI;
    Commands *cmd;
    Comms *comm;    
    QByteArray moduleCommandPrefix;

    AbstractSpecification* moduleSpecification = nullptr;
    Graphics *theme;

private:
    ModuleDockWidget *dockWidgetWindow;
    WidgetControlModule *moduleControlWidget;
    bool moduleDescriptionShown = false;
    bool dockWinCreated = false;
    bool moduleRestored = false;

public slots:
    void widgetControlClicked(ModuleStatus status);
    void widgetControlClickedWheel(QMouseEvent *event);
    void held(bool held);

signals:
    void moduleCreated();
    void holdClicked(bool held);
    void loadModuleLayoutAndConfig(QString moduleName);
    void blockConflictingModules(QString moduleName, int resources);
    void releaseConflictingModules(QString moduleName, int resources);
    void moduleDescription(QString name, QList<QString> labels, QList<QString> values);

};

#endif // ABSTRACTMODULE_H
