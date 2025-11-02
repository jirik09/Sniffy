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

    //void saveGeometry(QSettings &layout);
    // Save geometry entries into an in-memory map (key -> QByteArray)
    // so callers can avoid creating temporary QSettings files when exporting sessions.
    void saveGeometry(QMap<QString, QByteArray> &layoutMap);
    //void restoreGeometry(QSettings &layout);
    // Restore geometry from an in-memory map (key -> QByteArray) so callers
    // don't need to create a temporary QSettings file when loading JSON
    // sessions. Keys are the same as used for QSettings: moduleName+objectName.
    void restoreGeometry(const QMap<QString, QByteArray> &layoutMap);

    void setDockWidgetWindow(ModuleDockWidget *dockWidget);
    void setModuleControlWidget(WidgetControlModule *scpWidget);
    void setComms(Comms *communication);

    QByteArray getCommandPrefix();
    void hideModuleStatus();
    void moduleRestoredHidden();
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

    // GPIO mask getters (delegated to specification)
    quint32 getGpioMaskA() const { return moduleSpecification ? moduleSpecification->getGpioMaskA() : 0u; }
    quint32 getGpioMaskB() const { return moduleSpecification ? moduleSpecification->getGpioMaskB() : 0u; }
    quint32 getGpioMaskC() const { return moduleSpecification ? moduleSpecification->getGpioMaskC() : 0u; }
    quint32 getGpioMaskD() const { return moduleSpecification ? moduleSpecification->getGpioMaskD() : 0u; }

    bool isModuleRestored() const;
    void setModuleRestored(bool value);

    // Accessor for the module control widget (used by MainWindow for UI tweaks)
    WidgetControlModule* getModuleControlWidget() const { return moduleControlWidget; }

protected:
    QString moduleName;
    QString moduleIconURI;
    Commands *cmd;
    Comms *comm;    
    QByteArray moduleCommandPrefix;

    AbstractSpecification* moduleSpecification = nullptr;

private:
    ModuleDockWidget *dockWidgetWindow;
    WidgetControlModule *moduleControlWidget;
    bool moduleDescriptionShown = false;
    bool dockWinCreated = false;
    bool moduleRestored = false;

public slots:
    void widgetControlClicked(ModuleStatus status);
    void widgetHideWindowClicked();
    void widgetControlClickedWheel(QMouseEvent *event);
    void held(bool held);

signals:
    void moduleCreated();
    void holdClicked(bool held);
    void blockConflictingModules(QString moduleName, int resources);
    void releaseConflictingModules(QString moduleName, int resources);
    void moduleDescription(QString name, QList<QString> labels, QList<QString> values);
    // Emitted when the module's display name changes (e.g., Device renamed after handshake)
    void moduleNameChanged(const QString &name);
    // Emitted when showModuleControl is called, used to trigger delayed session restoration
    void moduleControlShown(const QString &name);

};

#endif // ABSTRACTMODULE_H
