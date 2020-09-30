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
   deviceConnectButton->setText("Connect",0);
   deviceConnectButton->setDisabledButton(true,0);
   deviceConnectButton->setText("Scan",1);

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

  // WidgetSeparator *scopeParameters = new WidgetSeparator(WidgetSpecification,"Scope Parameters");
  // verticalLayoutSpecification->addWidget(scopeParameters);

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
  //  qDebug() << "update specification got to GUI"
    QString devicePreviewImage =":/graphics/graphics/"+spec->device+".png";

    if(QFileInfo::exists(devicePreviewImage)){
        ui->widget_device->setStyleSheet("image: url(:/graphics/graphics/"+spec->device+".png);");
    }else{
        ui->widget_device->setStyleSheet("image: url(:/graphics/graphics/unknown_device.png);");
    }

    deviceParameters->show();
    labelMCU->show();
    labelMCU->setValue(spec->MCU);
    labelFWVer->show();
    labelFWVer->setValue(spec->FW_Version);
    labelHALVer->show();
    labelHALVer->setValue(spec->HAL_Version);
    labelRTOSVer->show();
    labelRTOSVer->setValue(spec->FREE_RTOS_Version);
    labelCoreFreq->show();
    labelCoreFreq->setValue(QString::number(spec->CoreClock/1000000) + "MHz");
}

void DeviceWindow::hideSpecification(){
    ui->widget_device->setStyleSheet("image: none;");
    ui->widget_device->setStyleSheet("image: url(:/graphics/graphics/no_device.png);");
    deviceParameters->hide();
    labelMCU->hide();
    labelFWVer->hide();
    labelHALVer->hide();
    labelRTOSVer->hide();
    labelCoreFreq->hide();
}

