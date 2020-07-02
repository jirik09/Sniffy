#include "devicemediator.h"

DeviceMediator::DeviceMediator(QObject *parent) : QObject(parent)
{
    deviceSpec = new DeviceSpec();
    commands = new Commands();
    communication = new Comms();    

    connect(communication,SIGNAL(devicesScaned(QList<DeviceDescriptor>)),this,SLOT(newDeviceList(QList<DeviceDescriptor>)),Qt::QueuedConnection);

    /* Create modules */
    modules = createModulesList();

    connect(scan,&Device::ScanDevicesGUI,this,&DeviceMediator::ScanDevices);
    connect(scan,&Device::updateSpecGUIGUI,this,&DeviceMediator::updateSpecGUI);
    connect(scan,&Device::openGUI,this,&DeviceMediator::open);
    connect(scan,&Device::closeGUI,this,&DeviceMediator::close);
}

/* Here's the created list of instantiated modules */
QList<QSharedPointer<AbstractModule>> DeviceMediator::createModulesList(){
    QList<QSharedPointer<AbstractModule>> modules;
    modules.append(QSharedPointer<AbstractModule> (scan = new Device()));
    modules.append(QSharedPointer<AbstractModule> (new Scope()));
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
    qDebug() << "scanned device lis received in device.cpp";
    scan->updateGUIDeviceList(deviceList);
}


void DeviceMediator::open(int deviceIndex){
    communication->open(deviceList.at(deviceIndex));

    while (communication->getIsOpen()==false) {
        QThread::msleep(500);
        qDebug() << "ERROR wait for comm to be opened";
    }
    isConnected = communication->getIsOpen();

    connect(communication,&Comms::newData,this,&DeviceMediator::parseData);
    connect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);

    communication->write(commands->SYSTEM+":"+commands->CONFIG_REQUEST+";");
    foreach(QSharedPointer<AbstractModule> mod, modules){
       mod->setComms(communication);
    }
}

void DeviceMediator::close(){
    if(isConnected){
        communication->close();
        foreach(QSharedPointer<AbstractModule> mod, modules){
           mod->closeModule();
        }
        isConnected = communication->getIsOpen();
    }

    disconnect(communication,&Comms::newData,this,&DeviceMediator::parseData);
    disconnect(communication,&Comms::communicationError,this,&DeviceMediator::handleError);
}

void DeviceMediator::handleError(QByteArray error){
    scan->errorHandler(error);
}

void DeviceMediator::parseData(QByteArray data){
    bool isDataPassed = false;

    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.right(data.length()-4);

    foreach(QSharedPointer<AbstractModule> mod, modules){
        if(dataHeader == mod->moduleCommandPrefix){
            mod->parseData(dataToPass);
            isDataPassed=true;
        }
    }
        //What if data belongs to voltmeter???????????
        //Solution1: each module has to know if it is running or not and handle data correctly.
        //Solution2: opeed modules will be handeled here

    if(dataHeader=="SYST"){
       parseSystemData(dataToPass);
        isDataPassed=true;
    }

    if(!isDataPassed){
       qDebug() << "ERROR: this data was not passed to any module" << data;
    }
}

void DeviceMediator::parseSystemData(QByteArray data){
 //   qDebug() << "data are in system parser device.cpp" << data;
    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec->parseSpecification(data);
        updateSpecGUI();
    }else if(feature=="ACK_"){
      //  qDebug() << "ACK";
    }else{
        qDebug() << "ERROR: unparsable data for system" << feature << " "<< data;
    }
}

bool DeviceMediator::getIsConnected() const
{
    return isConnected;
}

bool DeviceMediator::getIsSpecificationLoaded(){
    return deviceSpec->isSpecLoaded;
}

DeviceSpec* DeviceMediator::getDeviceSpecification(){
    return deviceSpec;
}

void DeviceMediator::updateSpecGUI(){
     qDebug() << "update specification got to GUI";

     if (getIsConnected() && getIsSpecificationLoaded()){
        scan->scanWindow->deviceParameters->show();
        scan->scanWindow->labelMCU->show();
        scan->scanWindow->labelMCU->setValue(getDeviceSpecification()->MCU);
        scan->scanWindow->labelFWVer->show();
        scan->scanWindow->labelFWVer->setValue(getDeviceSpecification()->FW_Version);
        scan->scanWindow->labelHALVer->show();
        scan->scanWindow->labelHALVer->setValue(getDeviceSpecification()->HAL_Version);
        scan->scanWindow->labelRTOSVer->show();
        scan->scanWindow->labelRTOSVer->setValue(getDeviceSpecification()->FREE_RTOS_Version);
        scan->scanWindow->labelCoreFreq->show();
        scan->scanWindow->labelCoreFreq->setValue(QString::number(getDeviceSpecification()->CoreClock/1000000) + "MHz");
    }else{
        scan->scanWindow->deviceParameters->hide();
        scan->scanWindow->labelMCU->hide();
        scan->scanWindow->labelFWVer->hide();
        scan->scanWindow->labelHALVer->hide();
        scan->scanWindow->labelRTOSVer->hide();
        scan->scanWindow->labelCoreFreq->hide();
    }
}



