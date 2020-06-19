/*
Main window of the application.

Left - centralWidget:
- top: logo from ui file.
- Features widgets are added here and act as a buttons
- separator
- widgetFooter with settings

Right - area for dock widgets
- already has DockWidged_device which is will show connection to board and its parameters on the righ
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup left menu with features separators etc
    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_features->addWidget(sep);

    /* BUDE HROMADA DUPLICIT - REFACTOR */

    WidgetModule_scope = new WidgetModule(ui->centralwidget,"Oscilloscope");
    ui->verticalLayout_features->addWidget(WidgetModule_scope);
    WidgetModule_scope->setStatus(ModuleStatus::STOP);
    WidgetModule_scope->hide();

    WidgetFeature_gen = new WidgetModule(ui->centralwidget,"Signal generator");
    ui->verticalLayout_features->addWidget(WidgetFeature_gen);
    WidgetFeature_gen->setStatus(ModuleStatus::STOP);

    WidgetFeature_counter = new WidgetModule(ui->centralwidget,"Counter");
    ui->verticalLayout_features->addWidget(WidgetFeature_counter);
    WidgetFeature_counter->setStatus(ModuleStatus::STOP);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_features->addItem(verticalSpacer);

    WidgetSeparator *sepa = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_features->addWidget(sepa);

    //footer widget
    WidgetFooter *footer = new WidgetFooter();
    ui->verticalLayout_features->addWidget(footer);

    connect(footer->getPushButtonSize(),SIGNAL(clicked()),this,SLOT(setMenuSize()));

    ui->dockWidget_device->setWindowTitle("Device");

    scpWindow = new WindowScope(dockWidget_scope);
    dockWidget_scope = new ModuleDockWidget(this,"Oscilloscope");
    dockWidget_scope->setWidget(scpWindow);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_scope);

  /*  ModuleDockWidget *myDock = new ModuleDockWidget("Hokus",this,);
    myDock->setWindowTitle("Hokus");
    myDock->setFloating(false);
    myDock->setMinimumSize(QSize(250, 250));
    myDock->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), myDock);
*/
  /*  dockWidget_scopeA=new QDockWidget(this);
    dockWidget_scopeA->setObjectName("dockWidget_scope");
    dockWidget_scopeA->setWindowTitle("Oscilloscope");
    dockWidget_scopeA->setMinimumSize(QSize(250, 250));
    dockWidget_scopeA->setFloating(false);
    dockWidget_scopeA->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_scopeA);

    dockWidget_scopeA->setWidget(scpWindow);
    dockWidget_scopeA->hide();*/


    dockWidget_counter=new QDockWidget(this);
    dockWidget_counter->setObjectName("dockWidget_counter");
    dockWidget_counter->setWindowTitle("Counter");
    dockWidget_counter->setMinimumSize(QSize(250, 250));
    dockWidget_counter->setFloating(false);
    dockWidget_counter->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_counter);
    cnt = new WindowCounter(dockWidget_counter);


    QVBoxLayout *horizontalLayout;
    horizontalLayout = new QVBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

   // horizontalLayout->addWidget(scpWindow);


    horizontalLayout->addWidget(cnt);
    dockWidget_counter->setWidget(cnt);
    dockWidget_counter->hide();


    //********************* Insert buttons and labels into central widget **************************
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);  //horizontal , vertical
    scrollAreaSpecification = new QScrollArea();
    scrollAreaSpecification->setWidgetResizable(true);
    scrollAreaSpecification->setSizePolicy(sizePolicy);

    WidgetSpecification = new QWidget();
    WidgetSpecification->setSizePolicy(sizePolicy);
    verticalLayoutSpecification = new QVBoxLayout(WidgetSpecification);

    scrollAreaSpecification->setContentsMargins(0,0,0,0);
    verticalLayoutSpecification->setSpacing(3);
    verticalLayoutSpecification->setContentsMargins(5,0,5,0);

    scrollAreaSpecification->setWidget(WidgetSpecification);
    ui->verticalLayout_specification->addWidget(scrollAreaSpecification);

    //add items - only specified widgets like buttons, dials, labels, separators....
    WidgetSeparator *availableDevices = new WidgetSeparator(WidgetSpecification,"Available devices");
    verticalLayoutSpecification->addWidget(availableDevices);

    deviceSelection  = new WidgetSelection(WidgetSpecification);
    verticalLayoutSpecification->addWidget(deviceSelection);

    deviceConnectButton = new WidgetButtons(WidgetSpecification,2);
    deviceConnectButton->setText("Connect",0);
    deviceConnectButton->setText("Scan",1);
    connect(deviceConnectButton,SIGNAL(clicked(int)),this,SLOT(deviceConnection(int)));
    verticalLayoutSpecification->addWidget(deviceConnectButton);

    deviceParameters = new WidgetSeparator(WidgetSpecification,"Device Parameters");
    verticalLayoutSpecification->addWidget(deviceParameters);
    labelMCU = new WidgetLabel(WidgetSpecification,"MCU");
    verticalLayoutSpecification->addWidget(labelMCU);
    labelCoreFreq = new WidgetLabel(WidgetSpecification,"Core Frequency");
    verticalLayoutSpecification->addWidget(labelCoreFreq);
    labelFWVer = new WidgetLabel(WidgetSpecification,"FW");
    verticalLayoutSpecification->addWidget(labelFWVer);
    labelRTOSVer = new WidgetLabel(WidgetSpecification,"Free RTOS");
    verticalLayoutSpecification->addWidget(labelRTOSVer);
    labelHALVer = new WidgetLabel(WidgetSpecification,"ST HAL");
    verticalLayoutSpecification->addWidget(labelHALVer);


    WidgetSeparator *scopeParameters = new WidgetSeparator(WidgetSpecification,"Scope Parameters");
    verticalLayoutSpecification->addWidget(scopeParameters);

    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutSpecification->addItem(verticalSpacer);
    //*************************** end adding widgets to specification area *************************

    device = new Device(this);
    connect(device,SIGNAL(updateDeviceList(QList<DeviceDescriptor>)),this,SLOT(updateGUIDeviceList(QList<DeviceDescriptor>)));
    connect(device,SIGNAL(updateSpecfication()),this,SLOT(updateSpecGUI()));
    connect(device, &Device::fatalError,this,&MainWindow::errorHandler);


    //this code enables automatic scan at startup
  /*  device->ScanDevices();
    deviceConnectButton->disableAll();
    deviceSelection->addOption("Scanning...",0);
*/

    //pass scope pointer and pointer to module widget into scope window - this connects mediator to GUI
    device->getScope()->setModuleWindow(scpWindow);
    device->getScope()->setDockWidgetWindow(dockWidget_scope);
    device->getScope()->setModuleControlWidget(WidgetModule_scope);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMenuSize(){
    if(ui->centralwidget->geometry().width()>150){
       ui->centralwidget->setMinimumSize(100,200);
       ui->centralwidget->setMaximumSize(100,20000);
    }else{
       ui->centralwidget->setMinimumSize(250,200);
       ui->centralwidget->setMaximumSize(250,20000);
    }
}

void MainWindow::deviceConnection(int buttonIndex){
    if(buttonIndex==1){
        qDebug() << "scan clicked";
        device->ScanDevices();
        deviceConnectButton->disableAll();
        deviceSelection->addOption("Scanning...",0);

    }else if(buttonIndex == 0){
        QString btnText = deviceConnectButton->getText(buttonIndex);

        if(btnText.compare("Connect")==0){
            connectDevice(deviceSelection->getSelected());
            qDebug() << "connect clicked";

        }else if (btnText.compare("Disconnect")==0){
            disconnectDevice();
            qDebug() << "disconnect clicked";
        }
    }
}

void MainWindow::updateGUIDeviceList(QList<DeviceDescriptor> deviceList){
    QList<DeviceDescriptor> list = deviceList;
    DeviceDescriptor devStr;
    int i = 0;

    deviceSelection->clear();
    foreach(devStr, list){
        deviceSelection->addOption(devStr.deviceName + " (" + devStr.port + ")",i);
        i++;
    }

    if(deviceSelection->count()==0){
        deviceSelection->addOption("No devices were found",0);
        deviceConnectButton->setDisabledButton(false,1); //enable scan button
    }else if(deviceSelection->count()==1){
        connectDevice(0);
        deviceConnectButton->setText("Disconnect",0);
        deviceConnectButton->setDisabledButton(false,0); //enable disconnect button
    }else{
        deviceConnectButton->setDisabledButton(false,0); //enable both
        deviceConnectButton->setDisabledButton(false,1);
    }
}

void MainWindow::connectDevice(int index){
    device->open(index);
    deviceConnectButton->setText("Disconnect",0);
    deviceConnectButton->setDisabledButton(true,1);//disable scan
}

void MainWindow::disconnectDevice(){
    device->close();
    dockWidget_scope->hide();
    deviceConnectButton->setText("Connect",0);
    deviceConnectButton->setDisabledButton(false,1);//enable scan
    updateSpecGUI();
}

void MainWindow::errorHandler(QByteArray error){
    QMessageBox messageBox;
    messageBox.critical(0,"Error","An error has occured:\n" + error + "\nPlease reconnect the device");
    messageBox.setFixedSize(500,200);
    disconnectDevice();
    deviceConnectButton->setDisabledButton(true,0);//disable connect
}

void MainWindow::updateSpecGUI(){
     qDebug() << "update specification got to GUI";

    if (device->getIsConnected() && device->getIsSpecificationLoaded()){
        deviceParameters->show();
        labelMCU->show();
        labelMCU->setValue(device->getDeviceSpecification()->MCU);
        labelFWVer->show();
        labelFWVer->setValue(device->getDeviceSpecification()->FW_Version);
        labelHALVer->show();
        labelHALVer->setValue(device->getDeviceSpecification()->HAL_Version);
        labelRTOSVer->show();
        labelRTOSVer->setValue(device->getDeviceSpecification()->FREE_RTOS_Version);
        labelCoreFreq->show();
        labelCoreFreq->setValue(QString::number(device->getDeviceSpecification()->CoreClock/1000000) + "MHz");
    }else{
        deviceParameters->hide();
        labelMCU->hide();
        labelFWVer->hide();
        labelHALVer->hide();
        labelRTOSVer->hide();
        labelCoreFreq->hide();
    }
}



