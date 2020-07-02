#include "device.h"

Device::Device(QObject *parent)
{
    Q_UNUSED(parent);
    scanWindow = new DeviceWindow();

    connect(scanWindow->deviceConnectButton,SIGNAL(clicked(int)),this,SLOT(deviceConnection(int)));

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
        scanWindow->deviceConnectButton->disableAll();
        scanWindow->deviceSelection->addOption("Scanning...",0);

    }else if(buttonIndex == 0){
        QString btnText = scanWindow->deviceConnectButton->getText(buttonIndex);

        if(btnText.compare("Connect")==0){
            connectDevice(scanWindow->deviceSelection->getSelected());
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

    scanWindow->deviceSelection->clear();
    foreach(devStr, list){
        scanWindow->deviceSelection->addOption(devStr.deviceName + " (" + devStr.port + ")",i);
        i++;
    }

    if(scanWindow->deviceSelection->count()==0){
        scanWindow->deviceSelection->addOption("No devices were found",0);
        scanWindow->deviceConnectButton->setDisabledButton(false,1); //enable scan button
    }else if(scanWindow->deviceSelection->count()==1){
        connectDevice(0);
        scanWindow->deviceConnectButton->setText("Disconnect",0);
        scanWindow->deviceConnectButton->setDisabledButton(false,0); //enable disconnect button
    }else{
        scanWindow->deviceConnectButton->setDisabledButton(false,0); //enable both
        scanWindow->deviceConnectButton->setDisabledButton(false,1);
    }
}

void Device::connectDevice(int index){
    emit openGUI(index);
    scanWindow->deviceConnectButton->setText("Disconnect",0);
    scanWindow->deviceConnectButton->setDisabledButton(true,1);//disable scan
}

void Device::disconnectDevice(){
    emit closeGUI();
    //dockWidget_scope->hide();
    scanWindow->deviceConnectButton->setText("Connect",0);
    scanWindow->deviceConnectButton->setDisabledButton(false,1);//enable scan
    emit updateSpecGUIGUI();
}

void Device::errorHandler(QByteArray error){
    QMessageBox messageBox;
    messageBox.critical(0,"Error","An error has occured:\n" + error + "\nPlease reconnect the device");
    messageBox.setFixedSize(500,200);
    disconnectDevice();
    scanWindow->deviceConnectButton->setDisabledButton(true,0);//disable connect
}

void Device::parseData(QByteArray){}
void Device::writeConfiguration(){}
void Device::startModule(){}
void Device::stopModule(){}
QWidget* Device::getWidget(){
    return scanWindow;
}
