#include "windowscan.h"
#include "ui_windowscan.h"

WindowScan::WindowScan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindowScan)
{
    ui->setupUi(this);

   //********************* Insert buttons and labels into Scan Window  **************************
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

   deviceConnectButton = new WidgetButtons(WidgetSpecification,2);
   deviceConnectButton->setText("Connect",0);
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

   WidgetSeparator *scopeParameters = new WidgetSeparator(WidgetSpecification,"Scope Parameters");
   verticalLayoutSpecification->addWidget(scopeParameters);

   verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
   verticalLayoutSpecification->addItem(verticalSpacer);

   //ui->dockWidget_device->setWindowTitle("Device");

   //*************************** end adding widgets to specification area *************************
}

WindowScan::~WindowScan()
{
    delete ui;
}

