#include "devicewindow.h"
#include "ui_devicewindow.h"

DeviceWindow::DeviceWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceWindow)
{
    ui->setupUi(this);

    // ********************* Insert buttons and labels into Scan Window  **************************
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);//horizontal , vertical
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
    deviceSelection->addOption("Scanning for available devices",-1);

    deviceConnectButton = new WidgetButtons(WidgetSpecification,2);
    deviceConnectButton->setObjectName("connectbutton");
    deviceConnectButton->setText("Connect",0);
    deviceConnectButton->setDisabledButton(true,0);
    deviceConnectButton->setText("Scan",1);

    verticalLayoutSpecification->addWidget(deviceConnectButton);

    modulesDescriptions = new QList<WidgetDesciptionExpand *>();

    devDesc = new WidgetDesciptionExpand(WidgetSpecification,"System");
    modulesDescriptions->append(devDesc);
    verticalLayoutSpecification->addWidget(devDesc);

    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutSpecification->addItem(verticalSpacer);

    // *************************** end adding widgets to specification area *************************

    hideSpecification();
}

DeviceWindow::~DeviceWindow()
{
    delete ui;
}

void DeviceWindow::showSpecification(DeviceSpec *spec){
    qDebug() << "update specification got to GUI";
    QString devicePreviewImage =":/graphics/graphics/"+spec->device+".png";

    if(QFileInfo::exists(devicePreviewImage)){
        ui->widget_device->setStyleSheet("image: url(:/graphics/graphics/"+spec->device+".png);");
    }else{
        ui->widget_device->setStyleSheet("image: url(:/graphics/graphics/unknown_device.png);");
    }

    QList<WidgetDesciptionExpand *>::iterator it;
    for (it = modulesDescriptions->begin(); it != modulesDescriptions->end(); ++it){
        (*it)->clearLabels();
    }

    devDesc->addLabel("MCU",spec->MCU);
    devDesc->addLabel("Core Frequency",QString::number(spec->CoreClock/1000000) + "MHz");
    devDesc->addLabel("FW",spec->FW_Version + " ("+spec->Build_Date+")");
    devDesc->addLabel("Free RTOS",spec->FREE_RTOS_Version);
    devDesc->addLabel("HAL",spec->HAL_Version);


    for (it = modulesDescriptions->begin(); it != modulesDescriptions->end(); ++it){
        (*it)->show();
    }
}

void DeviceWindow::hideSpecification(){
    QList<WidgetDesciptionExpand *>::iterator it;
    for (it = modulesDescriptions->begin(); it != modulesDescriptions->end(); ++it){
        (*it)->clearLabels();
        (*it)->hide();
    }
    ui->widget_device->setStyleSheet("image: none;");
    ui->widget_device->setStyleSheet("image: url(:/graphics/graphics/no_device.png);");
}

