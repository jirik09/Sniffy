#include "devicewindow.h"
#include "ui_devicewindow.h"
#include <QEvent>
#include <QMouseEvent>

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

    deviceSelection = new WidgetSelection(WidgetSpecification);
    verticalLayoutSpecification->addWidget(deviceSelection);
    deviceSelection->addOption("Scanning for available devices",-1);

    deviceConnectButton = new WidgetButtons(WidgetSpecification,2);
    deviceConnectButton->setObjectName("connectbutton");
    deviceConnectButton->setText("Connect",0);
    deviceConnectButton->setDisabledButton(true,0);
    deviceConnectButton->setText("Scan",1);

    verticalLayoutSpecification->addWidget(deviceConnectButton);

    modulesDescriptions = new QList<WidgetDesciptionExpand *>;

    descriptorsLayout = new QVBoxLayout();
    verticalLayoutSpecification->addLayout(descriptorsLayout);

    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutSpecification->addItem(verticalSpacer);

    // *************************** end adding widgets to specification area *************************

    hideSpecification();
    ui->widget_device->installEventFilter(this);
}

DeviceWindow::~DeviceWindow()
{
    delete ui;
}

void DeviceWindow::showSpecification(DeviceSpec *spec){
    //qDebug() << "update specification got to GUI";
    const QString baseName = spec->device; // e.g. Nucleo-F303RE
    const QString boardImg = Graphics::getBoardImage(baseName);
    ui->widget_device->setStyleSheet("image: url(" + boardImg + ");");
    currentDeviceBaseImage = baseName;
    showingPinout = false;

    QList<WidgetDesciptionExpand *>::iterator it;
    for (it = modulesDescriptions->begin(); it != modulesDescriptions->end(); ++it){
        (*it)->show();
    }
}

void DeviceWindow::hideSpecification(){
    QList<WidgetDesciptionExpand *>::iterator it;
    for (it = modulesDescriptions->begin(); it != modulesDescriptions->end(); ++it){
        (*it)->hide();
    }
    //ui->widget_device->setStyleSheet("image: none;");
    ui->widget_device->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"no_device.png);");
}

void DeviceWindow::addModuleDescription(QString name, QList<QString> labels, QList<QString> values)
{
    WidgetDesciptionExpand *newDesc = new WidgetDesciptionExpand(WidgetSpecification,name);
    for (int i = 0;i<labels.length() && i<values.length() ; i++) {
        newDesc->addLabel(labels.at(i),values.at(i));
    }

    modulesDescriptions->append(newDesc);
    descriptorsLayout->addWidget(newDesc);
}

void DeviceWindow::clearModuleDescriptions()
{
    QList<WidgetDesciptionExpand *>::iterator it;
    for (it = modulesDescriptions->begin(); it != modulesDescriptions->end(); ++it){
      //  (*it)->clearLabels();
        (*it)->deleteLater();
    }
    verticalLayoutSpecification->removeItem(descriptorsLayout);
    verticalLayoutSpecification->removeItem(verticalSpacer);

    descriptorsLayout->deleteLater();
    descriptorsLayout = new QVBoxLayout();

    verticalLayoutSpecification->addLayout(descriptorsLayout);
    verticalLayoutSpecification->addItem(verticalSpacer);

    modulesDescriptions->clear();
}

bool DeviceWindow::eventFilter(QObject *watched, QEvent *event){
    if(watched == ui->widget_device && event->type() == QEvent::MouseButtonRelease){
        auto *me = static_cast<QMouseEvent*>(event);
        if(me->button() == Qt::LeftButton && !currentDeviceBaseImage.isEmpty()){
            const QString pinoutPath = Graphics::getBoardPinoutImage(currentDeviceBaseImage);
            if(!pinoutPath.isEmpty()){
                if(!showingPinout){
                    ui->widget_device->setStyleSheet("image: url(" + pinoutPath + ");");
                    showingPinout = true;
                } else {
                    const QString normalPath = Graphics::getBoardImage(currentDeviceBaseImage);
                    ui->widget_device->setStyleSheet("image: url(" + normalPath + ");");
                    showingPinout = false;
                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}



