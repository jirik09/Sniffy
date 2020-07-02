#include "device.h"

Device::Device(QObject *parent)
{
    Q_UNUSED(parent);
    deviceWindow = new DeviceWindow();
    deviceSpec = new DeviceSpec();

    setCommandPrefix(cmd->SYSTEM);
    moduleName = "Device";

    connect(deviceWindow->deviceConnectButton,SIGNAL(clicked(int)),this,SLOT(deviceConnection(int)));

    //this code enables automatic scan at startup
    /*  device->ScanDevices();
       deviceConnectButton->disableAll();
       deviceSelection->addOption("Scanning...",0);
   */
}

void Device::deviceConnection(int buttonIndex){
    if(buttonIndex==1){
        qDebug() << "scan clicked";
        emit ScanDevicesGUI();
        deviceWindow->deviceConnectButton->disableAll();
        deviceWindow->deviceSelection->addOption("Scanning...",0);

    }else if(buttonIndex == 0){
        QString btnText = deviceWindow->deviceConnectButton->getText(buttonIndex);

        if(btnText.compare("Connect")==0){
            connectDevice(deviceWindow->deviceSelection->getSelected());
            qDebug() << "connect clicked";

        }else if (btnText.compare("Disconnect")==0){
            disconnectDevice();
            qDebug() << "disconnect clicked";
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
    emit openGUI(index);
    deviceWindow->deviceConnectButton->setText("Disconnect",0);
    deviceWindow->deviceConnectButton->setDisabledButton(true,1);//disable scan
}

void Device::disconnectDevice(){
    emit closeGUI();
    //dockWidget_scope->hide();
    deviceWindow->deviceConnectButton->setText("Connect",0);
    deviceWindow->deviceConnectButton->setDisabledButton(false,1);//enable scan
    emit updateSpecGUIGUI();
}

void Device::errorHandler(QByteArray error){
    QMessageBox messageBox;
    messageBox.critical(0,"Error","An error has occured:\n" + error + "\nPlease reconnect the device");
    messageBox.setFixedSize(500,200);
    disconnectDevice();
    deviceWindow->deviceConnectButton->setDisabledButton(true,0);//disable connect
}

void Device::parseData(QByteArray data){
    QByteArray feature = data.left(4);
    data.remove(0,4);

    if(feature=="CFG_"){
        deviceSpec->parseSpecification(data);
        deviceWindow->showSpecification(deviceSpec);
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
