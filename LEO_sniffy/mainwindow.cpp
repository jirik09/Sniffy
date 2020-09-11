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
#include "GUI/widgetfooter.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("LEO sniffy");

    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sep);

    createModulesWidgets();
    setupMainWindowComponents();
}

void MainWindow::createModulesWidgets(){
    deviceMediator = new DeviceMediator(this);

    QList<QSharedPointer<AbstractModule>> modulesList = deviceMediator->getModulesList();
    QSharedPointer<AbstractModule> module;
    QString moduleName;

    int listSize = modulesList.size();
    WidgetControlModule *WidgetModule[listSize];
    ModuleDockWidget *dockWidget[listSize];

    int index;
    foreach(module, modulesList){
        index = modulesList.indexOf(module);
        moduleName = module->getModuleName();

        WidgetModule[index] = new WidgetControlModule(ui->centralwidget, moduleName);
        ui->verticalLayout_modules->addWidget(WidgetModule[index]);
        WidgetModule[index]->setStatus(ModuleStatus::STOP);
        WidgetModule[index]->hide();

        dockWidget[index] = new ModuleDockWidget(this, moduleName);

        module->setDockWidgetWindow(dockWidget[index]);
        module->setModuleControlWidget(WidgetModule[index]);

        dockWidget[index]->setWidget(module->getWidget());

        addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget[index]);
    }

    deviceMediator->ShowDeviceModule();
}

void MainWindow::setupMainWindowComponents(){
    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sep);
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_modules->addItem(verticalSpacer);
    WidgetSeparator *sepa = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sepa);
    WidgetFooter *footer = new WidgetFooter();
    ui->verticalLayout_modules->addWidget(footer);

    connect(footer,&WidgetFooter::sizeClicked,this,&MainWindow::setMenuSize);

    QVBoxLayout *horizontalLayout;
    horizontalLayout = new QVBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

//    animation = new QPropertyAnimation(ui->centralwidget, "size", this);
//    animation->setDuration(355);
//    animation->setStartValue(QSize(250, ui->centralwidget->height()));
//    animation->setEndValue(QSize(90, ui->centralwidget->height()));
}

void MainWindow::setMenuSize(bool isWide){
    if(isWide){
        setMenuWide();
    }else{
        setMenuNarrow();
    }
}

void MainWindow::setMenuWide(){
    ui->centralwidget->setMinimumSize(250,200);
    ui->centralwidget->setMaximumSize(250,20000);

//    animation->setStartValue(QSize(90, ui->centralwidget->height()));
//    animation->setEndValue(QSize(250, ui->centralwidget->height()));
//    animation->start();
}

void MainWindow::setMenuNarrow(){
    ui->centralwidget->setMinimumSize(90,200);
    ui->centralwidget->setMaximumSize(90,20000);

//    animation->setStartValue(QSize(250, ui->centralwidget->height()));
//    animation->setEndValue(QSize(90, ui->centralwidget->height()));
//    animation->start();
}



MainWindow::~MainWindow()
{
    delete ui;
}

