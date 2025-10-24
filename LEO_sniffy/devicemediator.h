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
#include "resourcemanager.h"

// Forward declaration to avoid including the header here
class Authenticator;


class DeviceMediator : public QObject
{
    Q_OBJECT
public:
    explicit DeviceMediator(Authenticator *authenticator, QObject *parent = nullptr);
    ~DeviceMediator();
    QList<QSharedPointer<AbstractModule>> createModulesList();
    QList<QSharedPointer<AbstractModule>> getModulesList();   
    void ShowDeviceModule();
    bool getIsConnected() const;
    QString getDeviceName();
    QString getMcuId();

    ResourceSet getResourcesInUse() const;
    void setResourcesInUse(ResourceSet res);
    void disableModules();
    void closeModules();

signals:
    void loadLayoutUponOpen(QString Devicename);
    void saveLayoutUponExit(void);

private:
    QList<DeviceDescriptor> deviceList;
    QList<QSharedPointer<AbstractModule>> modules;
    Device *device;  

    Comms *communication;

    bool isConnected = false;
    int currentDeviceIndex = -1;

    // Manager handling legacy resources + GPIO masks aggregation and conflicts
    ResourceManager resourceManager;

    // Shared async authenticator (runs in GUI thread, non-blocking)
    Authenticator *authenticator = nullptr;

private slots:
    void parseData(QByteArray data);
    void newDeviceList(QList<DeviceDescriptor> deviceList);
    void handleError(QByteArray error);
    void ScanDevices();
    void openDevice(int deviceIndex);
    void disconnectDevice();
    void blockConflictingModulesCallback(QString moduleName, int resources);
    void releaseConflictingModulesCallback(QString moduleName, int resources);
    void onDeviceSpecificationReady();

public slots:
    void close();
    void closeApp();
    void reopenDeviceAfterLogin();
};

#endif // DEVICE_H
