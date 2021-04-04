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

#include "GUI/widgetcontrolmodule.h"
#include "GUI/widgetbuttons.h"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("LEO sniffy");
    sett = new SettingsDialog(this);

    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sep);

    createModulesWidgets();
    setupMainWindowComponents();

    connect(deviceMediator,&DeviceMediator::loadLayout,this,&MainWindow::loadLayout,Qt::DirectConnection);
}

void MainWindow::createModulesWidgets(){
    deviceMediator = new DeviceMediator(this);
    deviceMediator->passSettings(sett);

    modulesList = deviceMediator->getModulesList();
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
        connect(module.data(),&AbstractModule::loadModuleLayoutAndConfig,this,&MainWindow::loadModuleLayoutAndConfigCallback);
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
    connect(footer,&WidgetFooter::settingsClicked,this,&MainWindow::openSettingDialog);

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

void MainWindow::openSettingDialog()
{
    sett->open();
    //   sett->show();
    //   sett->activateWindow();
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

void MainWindow::closeEvent (QCloseEvent *event)
{
    saveLayout();
    deviceMediator->closeApp();
    event->accept();
}

void MainWindow::saveLayout()
{
    if(deviceMediator->getIsConnected()){
        if(!sett->askToSaveSession())return;
        QSharedPointer<AbstractModule> module;
        layoutFile = QApplication::applicationDirPath() + "/sessions/"+deviceMediator->getDeviceName()+".lay";
        configFile = QApplication::applicationDirPath() + "/sessions/"+deviceMediator->getDeviceName()+".cfg";

        QSettings layout(layoutFile, QSettings::IniFormat);
        layout.setValue("geometry", saveGeometry());
        layout.setValue("windowState", saveState());

        foreach(module, modulesList){
            module->saveGeometry(layout);
        }

        QSettings settings(configFile, QSettings::IniFormat);
        foreach(module, modulesList){
            settings.setValue(module->getModuleName()+"config",module->getConfiguration());
            settings.setValue(module->getModuleName()+"status",(int)(module->getModuleStatus()));
        }
        settings.setValue("resourcesInUse",deviceMediator->getResourcesInUse());
    }
}

void MainWindow::loadLayout(QString deviceName)
{
    if(!sett->IsSessionRestoreRequest()) return;

    layoutFile = QApplication::applicationDirPath() + "/sessions/"+deviceName+".lay";
    configFile = QApplication::applicationDirPath() + "/sessions/"+deviceName+".cfg";
    QFile file(layoutFile);

    if(!file.exists()) return;

    QSettings layout(layoutFile, QSettings::IniFormat);
    QSettings settings(configFile, QSettings::IniFormat);
    restoreGeometry(layout.value("geometry").toByteArray());
    restoreState(layout.value("windowState").toByteArray());

    deviceMediator->setResourcesInUse(settings.value("resourcesInUse").toInt());
}

void MainWindow::loadModuleLayoutAndConfigCallback(QString moduleName)
{
    if(!sett->IsSessionRestoreRequest()) return;

    QString layoutFile;
    QString configFile;
    layoutFile = QApplication::applicationDirPath() + "/sessions/"+deviceMediator->getDeviceName()+".lay";
    configFile = QApplication::applicationDirPath() + "/sessions/"+deviceMediator->getDeviceName()+".cfg";
    QSharedPointer<AbstractModule> module;

    QFile file(layoutFile);
    QFile fileMod(configFile);

    if(!file.exists() || !fileMod.exists()) return;

    QSettings settings(configFile, QSettings::IniFormat);
    QSettings layout(layoutFile, QSettings::IniFormat);
    ModuleStatus status;
    QByteArray config;

    foreach(module, modulesList){
        if(module->getModuleName()==moduleName){
            if(!module->isModuleRestored()){
                status = (ModuleStatus)settings.value(module->getModuleName()+"status").toInt();
                config = settings.value(module->getModuleName()+"config").toByteArray();
                module->parseConfiguration(config);

                module->restoreGeometry(layout);
                module->setModuleStatus(status);
                if(status == ModuleStatus::PLAY || status == ModuleStatus::HIDDEN_PLAY || status == ModuleStatus::PAUSE){
                    /*if(status == ModuleStatus::PAUSE)
                        status = ModuleStatus::PLAY;*/
                    module->writeConfiguration();
                    module->startModule();
                }

                module->setModuleRestored(true);
            }
        }
    }
}

