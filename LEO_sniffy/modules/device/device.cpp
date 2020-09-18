#include "device.h"

Device::Device(QObject *parent)
{
    Q_UNUSED(parent);
    deviceWindow = new DeviceWindow();
    deviceSpec = new DeviceSpec();

    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = cmd->SYSTEM;
    moduleName = "Device";
    moduleIconURI = ":/graphics/graphics/icon_not_connected.png";

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
            connectDevice(deviceWindow->deviceSelection->getSelected());
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
    foreach(devStr, list){
        deviceWindow->deviceSelection->addOption(devStr.deviceName + " (" + devStr.port + ")",i);
        i++;
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

void Device::connectDevice(int index){
    emit opened(index);
    deviceWindow->deviceConnectButton->setText("Disconnect",0);
    deviceWindow->deviceConnectButton->setDisabledButton(true,1);//disable scan
}

void Device::disconnectDevice(){
    emit closed();
    deviceWindow->deviceConnectButton->setText("Connect",0);
    deviceWindow->deviceConnectButton->setDisabledButton(false,1);//enable scan
    setIcon(":/graphics/graphics/icon_not_connected.png");
    setModuleName("Device");
}

void Device::errorHandler(QByteArray error){
    QMessageBox messageBox;
    messageBox.critical(0,"Error","An error has occured:\n" + error + "\nPlease reconnect the device");
    messageBox.setFixedSize(500,200);
    disconnectDevice();
    deviceWindow->hideSpecification();
    deviceWindow->deviceConnectButton->setDisabledButton(true,0);//disable connect
}

void Device::parseData(QByteArray data){
    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec->parseSpecification(data);
        deviceWindow->showSpecification(deviceSpec);
        setIcon(":/graphics/graphics/icon_connected.png");
        setModuleName(deviceSpec->device);
    }else if(feature=="ACK_"){
        //  qDebug() << "ACK";
    }else{
        qDebug() << "ERROR: unparsable data for system" << feature << " "<< data;
    }

}
void Device::writeConfiguration(){}
void Device::startModule(){}
void Device::stopModule(){}
QWidget* Device::getWidget(){
    return deviceWindow;
}
