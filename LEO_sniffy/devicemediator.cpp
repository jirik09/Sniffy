#include "devicemediator.h"

DeviceMediator::DeviceMediator(QObject *parent) : QObject(parent)
{
    communication = new Comms();

    connect(communication,SIGNAL(devicesScaned(QList<DeviceDescriptor>)),this,SLOT(newDeviceList(QList<DeviceDescriptor>)),Qt::QueuedConnection);

    //create device module
    modulesControlWidgets.append(new WidgetControlModule(nullptr,"Device"));
    modulesDockWidgets.append(new ModuleDockWidget(nullptr, "Device"));
    modules.append(QSharedPointer<AbstractModule> (device = new Device(this,nullptr,nullptr,modulesControlWidgets.last(),modulesDockWidgets.last())));

    connect(device,&Device::ScanDevices,this,&DeviceMediator::ScanDevices);
    connect(device,&Device::openDevice,this,&DeviceMediator::open);
    connect(device,&Device::closeDevice,this,&DeviceMediator::close);
}

QList<QSharedPointer<AbstractModule>> DeviceMediator::createModulesList(){
    QList<QSharedPointer<AbstractModule>> modules;
    //modules.append(QSharedPointer<AbstractModule> (device = new Device(this)));
    modules.append(QSharedPointer<AbstractModule> (new Scope(this)));
    modules.append(QSharedPointer<AbstractModule> (new Counter(this)));
    modules.append(QSharedPointer<AbstractModule> (new TemplateModule(this)));
    return modules;
}

QList<QSharedPointer<AbstractModule>> DeviceMediator::getModulesList(){
    return modules;
}

void DeviceMediator::ScanDevices(){
    communication->scanForDevices();
}

void DeviceMediator::newDeviceList(QList<DeviceDescriptor> deviceList){
    this->deviceList = deviceList;
    //qDebug() << "scanned device list received in device.cpp";
    device->updateGUIDeviceList(deviceList);
}

void DeviceMediator::open(int deviceIndex){
    communication->open(deviceList.at(deviceIndex));

    QThread::msleep(50);
    int i = 0;
    while (communication->getIsOpen()==false) {
        QThread::msleep(500);
        i++;
        if(i>5){
            device->errorHandler("Device cannot be opened");
            break;
        }
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();
    if(isConnected){
        buildModules(deviceList.at(deviceIndex).deviceName,communication);
        emit loadLayout(deviceList.at(deviceIndex).deviceName);
    }

    if(isConnected){
        connect(communication,&Comms::newData,this,&DeviceMediator::parseData);
        connect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);
    }
}

void DeviceMediator::close(){
    if(isConnected){
        foreach(QSharedPointer<AbstractModule> mod, modules){
            mod->disableModule();
        }
        communication->close();
        isConnected = false;
    }
    ShowDeviceModule();

    disconnect(communication,&Comms::newData,this,&DeviceMediator::parseData);
    disconnect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);
}

void DeviceMediator::handleError(QByteArray error){
    device->errorHandler(error);
}

void DeviceMediator::parseData(QByteArray data){
    bool isDataPassed = false;
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    //What if data belongs to voltmeter and scope???????????
    //Solution1: each module has to know if it is running or not and handle data correctly.
    foreach(QSharedPointer<AbstractModule> module, modules){
        if(dataHeader == module->getCommandPrefix()){
            module->parseData(dataToPass);
            isDataPassed=true;
        }
    }
    if(!isDataPassed){
        qDebug() << "ERROR: this data was not passed to any module" << data;
    }
}

void DeviceMediator::ShowDeviceModule(){
    device->showModuleWindow();
    device->showModuleControl();
    device->hideModuleStatus();
}

bool DeviceMediator::getIsConnected() const
{
    return isConnected;
}

QString DeviceMediator::getDeviceName(){
    return device->getName();
}

void DeviceMediator::buildModules(QString deviceName,Comms *communication)
{
    bool configExist = false;
    QString layoutFile;
    QString configFile;
    QSettings *settings = nullptr;
    QSettings *layout = nullptr;
    layoutFile = QApplication::applicationDirPath() + "/sessions/"+deviceName+".lay";
    configFile = QApplication::applicationDirPath() + "/sessions/"+deviceName+".cfg";
    QSharedPointer<AbstractModule> module;

    QFile file(layoutFile);
    QFile fileMod(configFile);

    if(file.exists() && fileMod.exists()){
        configExist = true;
        qDebug () << "seesion found";
    }

    if(configExist){
        settings = new QSettings(configFile, QSettings::IniFormat);
        layout = new QSettings(layoutFile, QSettings::IniFormat);
    }
    ModuleStatus status = ModuleStatus::STOP;
    QByteArray config = nullptr;


    //set communication for device
    modules.at(0)->setComms(communication);

    //init counter and all its stuff
    if(configExist){
        status = (ModuleStatus)settings->value("Counterstatus").toInt();
        config = settings->value("Counterconfig").toByteArray();
    }
    modulesControlWidgets.append(new WidgetControlModule(nullptr,"Counter"));
    modulesDockWidgets.append(new ModuleDockWidget(nullptr, "Counter"));
    module = QSharedPointer<AbstractModule>(new Counter(this,config,communication,modulesControlWidgets.last(),modulesDockWidgets.last()));
    modules.append(module);
    if(configExist)
        module->restoreGeometry(*layout);
    handleStatus(module,status);

    //init scope and all its stuff
    if(configExist){
        status = (ModuleStatus)settings->value("Oscilloscopestatus").toInt();
        config = settings->value("Oscilloscopeconfig").toByteArray();
    }
    modulesControlWidgets.append(new WidgetControlModule(nullptr,"Oscilloscope"));
    modulesDockWidgets.append(new ModuleDockWidget(nullptr, "Oscilloscope"));
    module = QSharedPointer<AbstractModule>(new Scope(this,config,communication,modulesControlWidgets.last(),modulesDockWidgets.last()));
    modules.append(module);
    if(configExist)
        module->restoreGeometry(*layout);
    handleStatus(module,status);


    emit modulesBuilt();
}

void DeviceMediator::handleStatus(QSharedPointer<AbstractModule> module, ModuleStatus status)
{

    if(status == ModuleStatus::PLAY || status == ModuleStatus::HIDDEN_PLAY || status == ModuleStatus::PAUSE){
        if(status == ModuleStatus::PAUSE)
            status = ModuleStatus::PLAY;
        module->writeConfiguration();
        module->startModule();
    }
    module->setModuleStatus(status);
}

