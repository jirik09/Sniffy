#include "devicewindow.h"
#include "ui_devicewindow.h"
#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>

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

    // Create vector pinout widget, overlaid inside widget_device
    pinoutWidget = new PinoutWidget(ui->widget_device);
    pinoutWidget->hide();
    // Initial size sync; kept in sync via Resize event in eventFilter
    pinoutWidget->setGeometry(ui->widget_device->rect());

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
    allPinFunctions.clear();
    pinFunctionModulesByDisplayName.clear();
    activeDisplayModules.clear();
    activePinFunctionModules.clear();
    pinoutWidget->setBoard(baseName);
    pinoutWidget->clearPinFunctions();
    pinoutWidget->setActiveModules(activePinFunctionModules);
    pinoutWidget->hide();

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
    ui->widget_device->setStyleSheet("image: url("+Graphics::tintedPath(Graphics::getCommonPath()+"no_device.png")+");");
    allPinFunctions.clear();
    pinFunctionModulesByDisplayName.clear();
    activeDisplayModules.clear();
    activePinFunctionModules.clear();
    if(pinoutWidget){
        pinoutWidget->clearPinFunctions();
        pinoutWidget->setActiveModules(activePinFunctionModules);
        pinoutWidget->hide();
    }
    showingPinout = false;
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

void DeviceWindow::addModulePinFunctions(const QString &moduleName, const QList<PinFunctionInfo> &pins)
{
    // Accumulate all functions; update pinout if already showing
    allPinFunctions.append(pins);

    QSet<QString> functionModules;
    for(const PinFunctionInfo &pin : pins){
        if(!pin.moduleName.isEmpty())
            functionModules.insert(pin.moduleName);
    }
    if(!functionModules.isEmpty())
        pinFunctionModulesByDisplayName.insert(moduleName, functionModules);

    rebuildActivePinFunctionModules();
    if(pinoutWidget){
        pinoutWidget->setPinFunctions(allPinFunctions);
        pinoutWidget->setActiveModules(activePinFunctionModules);
    }
}

void DeviceWindow::setModuleActive(const QString &moduleName, bool active)
{
    if(active)
        activeDisplayModules.insert(moduleName);
    else
        activeDisplayModules.remove(moduleName);

    rebuildActivePinFunctionModules();
    if(pinoutWidget)
        pinoutWidget->setActiveModules(activePinFunctionModules);
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

void DeviceWindow::rebuildActivePinFunctionModules()
{
    activePinFunctionModules.clear();
    for(auto it = pinFunctionModulesByDisplayName.constBegin(); it != pinFunctionModulesByDisplayName.constEnd(); ++it){
        if(activeDisplayModules.contains(it.key()))
            activePinFunctionModules.unite(it.value());
    }
}

bool DeviceWindow::eventFilter(QObject *watched, QEvent *event){
    if(watched == ui->widget_device){
        // Keep the vector pinout widget filling widget_device
        if(event->type() == QEvent::Resize && pinoutWidget){
            pinoutWidget->setGeometry(ui->widget_device->rect());
        }

        if(event->type() == QEvent::MouseButtonRelease){
            auto *me = static_cast<QMouseEvent*>(event);
            if(me->button() == Qt::LeftButton && !currentDeviceBaseImage.isEmpty()){
                // If the vector pinout can be shown, use it; otherwise fall back to static PNG
                const bool hasVectorPinout = !m_pins_empty();
                const bool hasStaticPinout = !Graphics::getBoardPinoutImage(currentDeviceBaseImage).isEmpty();

                if(hasVectorPinout || hasStaticPinout){
                    if(!showingPinout){
                        if(hasVectorPinout){
                            // Show vector pinout overlay on top
                            pinoutWidget->setGeometry(ui->widget_device->rect());
                            pinoutWidget->show();
                            pinoutWidget->raise();
                        } else {
                            // Fallback: old static PNG behaviour
                            ui->widget_device->setStyleSheet(
                                "image: url(" + Graphics::getBoardPinoutImage(currentDeviceBaseImage) + ");");
                        }
                        showingPinout = true;
                    } else {
                        if(hasVectorPinout){
                            pinoutWidget->hide();
                        } else {
                            ui->widget_device->setStyleSheet(
                                "image: url(" + Graphics::getBoardImage(currentDeviceBaseImage) + ");");
                        }
                        showingPinout = false;
                    }
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

// Returns true if the pinout widget has board data loaded
bool DeviceWindow::m_pins_empty() const
{
    return pinoutWidget == nullptr || currentDeviceBaseImage.isEmpty();
}



