#include "device.h"

Device::Device(QObject *parent)
{
    Q_UNUSED(parent);
    deviceWindow = new DeviceWindow();
    moduleSpecification = new DeviceSpec();

    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = cmd->SYSTEM;
    moduleName = "Device";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_not_connected.png";

    connect(deviceWindow->deviceConnectButton,SIGNAL(clicked(int)),this,SLOT(deviceConnection(int)));
}

//this is GUI so it should be in deviceWindow.cpp
//(keep it here bacause it is simpler and no need to use signals)
void Device::deviceConnection(int buttonIndex){
    if(buttonIndex==1){
        emit ScanDevices();
        deviceWindow->deviceConnectButton->disableAll();
        deviceWindow->deviceSelection->addOption("Scanning...",0);

    }else if(buttonIndex == 0){
        QString btnText = deviceWindow->deviceConnectButton->getText(buttonIndex);

        if(btnText.compare("Connect")==0){
            connectDevice(deviceWindow->deviceSelection->getSelectedIndex());
        }else if (btnText.compare("Disconnect")==0){
            disconnectDevice();
            deviceWindow->hideSpecification();
        }
    }
}

void Device::updateGUIDeviceList(QList<DeviceDescriptor> deviceList){
    QList<DeviceDescriptor> list = deviceList;
    DeviceDescriptor devStr;
    int i = 0;

    deviceWindow->deviceSelection->clear();
    for (const DeviceDescriptor &devStr : list) {
        deviceWindow->deviceSelection->addOption(devStr.deviceName + " (" + devStr.port + ")", i);
        ++i;
    }
    if(deviceWindow->deviceSelection->count()==0){
        deviceWindow->deviceSelection->addOption("No devices were found",0);
        deviceWindow->deviceConnectButton->setDisabledButton(false,1); //enable scan button
    }else if(deviceWindow->deviceSelection->count()==1){
        connectDevice(0);
        deviceWindow->deviceConnectButton->setText("Disconnect",0);
        deviceWindow->deviceConnectButton->setDisabledButton(false,0); //enable disconnect button
    }else{
        deviceWindow->deviceConnectButton->setDisabledButton(false,0); //enable both
        deviceWindow->deviceConnectButton->setDisabledButton(false,1);
    }
}

void Device::addModuleDescription(QString name, QList<QString> labels, QList<QString> values)
{
    deviceWindow->addModuleDescription(name,labels,values);
}

void Device::connectDevice(int index){
    emit openDevice(index);
    deviceWindow->deviceConnectButton->setText("Disconnect",0);
    deviceWindow->deviceConnectButton->setDisabledButton(true,1);//disable scan
}

void Device::disconnectDevice(){
    deviceWindow->clearModuleDescriptions();
    emit closeDevice();
    deviceWindow->deviceConnectButton->setText("Connect",0);
    deviceWindow->deviceConnectButton->setDisabledButton(false,1);//enable scan
    setIcon(Graphics::getGraphicsPath()+"icon_not_connected.png");
    setModuleName("Device");
}

void Device::passSystemSpecificationToGUI()
{
    QList<QString> labels, values;;
    labels.append("MCU");
    values.append(deviceSpec->MCU);
    labels.append("Core frequency");
    values.append(QString::number(deviceSpec->CoreClock/1000000) + "MHz");
    labels.append("FW");
    values.append(deviceSpec->FW_Version + " ("+deviceSpec->Build_Date+")");
    labels.append("Free RTOS");
    values.append(deviceSpec->FREE_RTOS_Version);
    labels.append("HAL");
    values.append(deviceSpec->HAL_Version);
    deviceWindow->addModuleDescription("System Info",labels,values);
}

void Device::clearAllModuleDescriptions()
{
    if (deviceWindow) {
        deviceWindow->clearModuleDescriptions();
    }
}

void Device::errorHandler(QByteArray error){
    QMessageBox messageBox;
    messageBox.critical(0,"Error","An error has occured:\n" + error + "\nPlease reconnect the device");
    messageBox.setFixedSize(500,200);
    disconnectDevice();
    deviceWindow->hideSpecification();
    deviceWindow->deviceSelection->clear();
    deviceWindow->deviceSelection->addOption("Connection lost", -1);
    deviceWindow->deviceConnectButton->setDisabledButton(true,0);//disable connect
}

void Device::parseData(QByteArray data){
    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec = static_cast<DeviceSpec*>(moduleSpecification);
        deviceSpec->parseSpecification(data);
        passSystemSpecificationToGUI();
        deviceWindow->showSpecification(deviceSpec);
        setIcon(Graphics::getGraphicsPath()+"icon_connected.png");
        setModuleName(static_cast<DeviceSpec*>(moduleSpecification)->device);
        emit deviceSpecificationReady(); //try to refresh authentication
    }else if(feature=="ACK_"){
        //  qDebug() << "ACK";
    }else{
        qDebug() << "WARNING: Device error message received: " <<ErrorList::GetErrMessage((uint8_t)(feature.at(2)));//<< feature << " "<< data;
    }
}

void Device::writeConfiguration(){}
void Device::parseConfiguration(QByteArray config){Q_UNUSED(config)}
QByteArray Device::getConfiguration(){return "none";}

void Device::startModule(){}
void Device::stopModule(){}


QWidget* Device::getWidget(){
    return deviceWindow;
}

QString Device::getName()
{
    return static_cast<DeviceSpec*>(moduleSpecification)->device;
}

QString Device::getMcuId()
{
    return static_cast<DeviceSpec*>(moduleSpecification)->MCU_ID;
}
