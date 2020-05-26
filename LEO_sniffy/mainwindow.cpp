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

    communication = new Comms();

    //setup left menu with features separators etc
    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_features->addWidget(sep);

    WidgetFeature_scope = new WidgetFeature(ui->centralwidget,FeatureIcon::SCOPE,"Oscilloscope");
    ui->verticalLayout_features->addWidget(WidgetFeature_scope);
    WidgetFeature_scope->setStatus(FeatureStatus::STOP);

    WidgetFeature_gen = new WidgetFeature(ui->centralwidget,FeatureIcon::SCOPE,"Signal generator");
    ui->verticalLayout_features->addWidget(WidgetFeature_gen);
    WidgetFeature_gen->setStatus(FeatureStatus::STOP);


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


    dockWidget_scope=new QDockWidget(this);
    dockWidget_scope->setObjectName("dockWidget_scope");
    dockWidget_scope->setWindowTitle("Oscilloscope");
    dockWidget_scope->setMinimumSize(QSize(250, 250));
    dockWidget_scope->setFloating(false);
    dockWidget_scope->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_scope);
    scp = new WindowScope(dockWidget_scope);

    QWidget *title_bar = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    title_bar->setLayout(layout);


    //customized title bar widget
    layout->addWidget(new QLabel("scope window"));
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addWidget(new QPushButton("up"));
    layout->addWidget(new QPushButton("down"));
    layout->addWidget(new QPushButton("Exit"));
    dockWidget_scope->setTitleBarWidget(title_bar);







    QVBoxLayout *horizontalLayout;
    horizontalLayout = new QVBoxLayout(dockWidget_scope);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->addWidget(scp);
    dockWidget_scope->setWidget(scp);
    dockWidget_scope->raise();


    connect(WidgetFeature_scope->getPushButton(),SIGNAL(clicked()),this,SLOT(openScope()));



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
    verticalLayoutSpecification->setContentsMargins(10,0,10,0);

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



    findDevices();



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

void MainWindow::openScope(){

    if(dockWidget_scope->isHidden()){
        dockWidget_scope->show();
    }else{
        dockWidget_scope->raise();
    }

}

void MainWindow::deviceConnection(int buttonIndex){
    if(buttonIndex==1){
        qDebug() << "scan clicked";
        findDevices();
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

void MainWindow::findDevices(){
    deviceConnectButton->disableAll();
    deviceSelection->clear();
    QList<device_descriptor> list = *communication->scanForDevices();
    device_descriptor devStr;
    int i = 0;
    foreach(devStr, list){
        deviceSelection->addOption(devStr.deviceName + " (" + devStr.port + ")",i);
        i++;
    }

    device = new Device(this,communication);

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
    device->loadHWSpecification();
    deviceConnectButton->setText("Disconnect",0);
    deviceConnectButton->setDisabledButton(true,1);//disable scan
    QThread::msleep(5000);
    qDebug() << "when it appears";
 //   updateSpecGUI();
}

void MainWindow::disconnectDevice(){
    device->close();
    deviceConnectButton->setText("Connect",0);
    deviceConnectButton->setDisabledButton(false,1);//enable scan
   // updateSpecGUI();
}

void MainWindow::updateSpecGUI(){
    if (device->getIsConnected()){

        int i = 0;
        while (!device->getIsSpecificationLoaded()) {
            QThread::msleep(10);
            if(i++==1000){
                qDebug() << "ERROR : specification was not laoded";
                return;
            }
        }
        //if(device->getIsSpecificationLoaded())
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



