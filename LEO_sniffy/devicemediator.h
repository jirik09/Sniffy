#ifndef DEVICE_H
#define DEVICE_H

#include "communication/comms.h"
#include "communication/commands.h"
#include "settingsdialog.h"
#include "modules/device/device.h"
#include "modules/scope/scope.h"
#include "modules/counter/counter.h"
#include "modules/voltmeter/voltmeter.h"
#include "modules/syncpwm/syncpwm.h"
#include "modules/arbgenerator/arbgenerator.h"
#include "modules/patterngenerator/patterngenerator.h"
#include "modules/pwmgenerator/pwmgenerator.h"
#include "modules/voltagesource/voltagesource.h"
#include "modules/template/templatemodule.h"


class DeviceMediator : public QObject
{
    Q_OBJECT
public:
    explicit DeviceMediator(QObject *parent = nullptr);
    QList<QSharedPointer<AbstractModule>> createModulesList();
    QList<QSharedPointer<AbstractModule>> getModulesList();   
    void ShowDeviceModule();
    bool getIsConnected() const;
    QString getDeviceName();

    int getResourcesInUse() const;
    void setResourcesInUse(int value);
    void passSettings(SettingsDialog *sett);

signals:
    void loadLayout(QString Devicename);

private:
    QList<DeviceDescriptor> deviceList;
    QList<QSharedPointer<AbstractModule>> modules;
    Device *device;
    SettingsDialog *sett;    

    Comms *communication;

    bool isConnected = false;
    int resourcesInUse = 0;

private slots:
    void parseData(QByteArray data);
    void newDeviceList(QList<DeviceDescriptor> deviceList);
    void handleError(QByteArray error);
    void ScanDevices();
    void open(int deviceIndex);
    void disableModules();
    void blockConflictingModulesCallback(QString moduleName, int resources);
    void releaseConflictingModulesCallback(QString moduleName, int resources);

public slots:
    void close();
    void closeApp();
};

#endif // DEVICE_H
