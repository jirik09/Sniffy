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
    this->setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));

    ui->widget_3->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"logo_gray_full.png);");
    const auto &pal = Graphics::palette();
    ui->centralwidget->setStyleSheet("QWidget{background-color:"+pal.windowWidget+";}"
                                     "QWidget#centralwidget{border-right: 1px solid"+pal.textLabel+";}");

    setWindowTitle("LEO sniffy");
    sett = new SettingsDialog(this);
    logindial = new LoginDialog(this);

    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    //ui->widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->verticalLayout_modules->addWidget(sep);

    createModulesWidgets();
    setupMainWindowComponents();

    connect(deviceMediator,&DeviceMediator::loadLayout,this,&MainWindow::loadLayout,Qt::DirectConnection);
    connect(deviceMediator,&DeviceMediator::saveLayout,this,&MainWindow::saveLayout,Qt::DirectConnection);

   // setMenuWide();
}

void MainWindow::createModulesWidgets(){
    deviceMediator = new DeviceMediator(this);

    modulesList = deviceMediator->getModulesList();
    QSharedPointer<AbstractModule> module;
    QString moduleName;

    int listSize = modulesList.size();
    std::vector<WidgetControlModule*> widgetModules;
    std::vector<ModuleDockWidget*> dockWidgets;
    widgetModules.resize(listSize, nullptr);
    dockWidgets.resize(listSize, nullptr);

    int index;
    for (const QSharedPointer<AbstractModule>& module : modulesList) {
        index = modulesList.indexOf(module);
        moduleName = module->getModuleName();
        if(index == 0 && moduleName == "Device"){
            deviceModule = module;
            // When device name changes after handshake (e.g., to NUCLEO-..), re-apply compact hiding if left menu is narrow
            connect(module.data(), &AbstractModule::moduleNameChanged, this, [this](const QString &){
                if (isLeftMenuNarrow) {
                    updateLeftMenuCompact(true);
                }
            });
        }

    widgetModules[index] = new WidgetControlModule(ui->centralwidget, moduleName);
    ui->verticalLayout_modules->addWidget(widgetModules[index]);
    widgetModules[index]->setStatus(ModuleStatus::STOP);
    widgetModules[index]->hide();

    dockWidgets[index] = new ModuleDockWidget(this, moduleName);

    module->setDockWidgetWindow(dockWidgets[index]);
    module->setModuleControlWidget(widgetModules[index]);

    dockWidgets[index]->setWidget(module->getWidget());

    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidgets[index]);
        connect(module.data(),&AbstractModule::loadModuleLayoutAndConfig,this,&MainWindow::loadModuleLayoutAndConfigCallback);
    }

    deviceMediator->ShowDeviceModule();
}

void MainWindow::setupMainWindowComponents(){
    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sep);
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_modules->addItem(verticalSpacer);
    WidgetSeparator *sepa = new WidgetSeparator(ui->centralwidget,"Ver 0.1");
    ui->verticalLayout_modules->addWidget(sepa);
    loginInfo = new WidgetLoginInfo();
    ui->verticalLayout_modules->addWidget(loginInfo);
    footer = new WidgetFooter();
    ui->verticalLayout_modules->addWidget(footer);


    connect(footer,&WidgetFooter::sizeClicked,this,&MainWindow::setMenuSize);
    connect(footer,&WidgetFooter::settingsClicked,this,&MainWindow::openSettingDialog);
    connect(loginInfo,&WidgetLoginInfo::loginDialogClicked,this,&MainWindow::openLoginDialog);
    connect(logindial,&LoginDialog::loginInfoChanged,this,&MainWindow::updateLoginInfo);

    QVBoxLayout *horizontalLayout;
    horizontalLayout = new QVBoxLayout();
    horizontalLayout->setObjectName("horizontalLayout");

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

void MainWindow::openLoginDialog()
{
    logindial->open();
}

void MainWindow::updateLoginInfo()
{
    loginInfo->updateInfo();
}

void MainWindow::openSettingDialog()
{
    sett->open();
}

void MainWindow::setMenuWide(){
    ui->centralwidget->setMinimumSize(250,200);
    ui->centralwidget->setMaximumSize(250,20000);
    isLeftMenuNarrow = false;
    updateLeftMenuCompact(false);
    if (loginInfo) loginInfo->setCompact(false);
}

void MainWindow::recoverLeftMenu(bool isWide)
{
    footer->setAppearance(isWide);
    setMenuSize(isWide);
}

void MainWindow::setMenuNarrow(){
    ui->centralwidget->setMinimumSize(90,200);
    ui->centralwidget->setMaximumSize(90,20000);
    isLeftMenuNarrow = true;
    updateLeftMenuCompact(true);
    if (loginInfo) loginInfo->setCompact(true);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    deviceMediator->closeApp();
    event->accept();
}

void MainWindow::saveLayout()
{
    if(deviceMediator->getIsConnected()){
        // Prevent creating ".cfg" / ".lay" with empty device name
        const QString devName = deviceMediator->getDeviceName();
        if(devName.isEmpty()) return;
        if(!CustomSettings::askToSaveSession())return;
        QSharedPointer<AbstractModule> module;
        layoutFile = QApplication::applicationDirPath() + "/sessions/"+devName+".lay";
        configFile = QApplication::applicationDirPath() + "/sessions/"+devName+".cfg";

        QSettings layout(layoutFile, QSettings::IniFormat);
        layout.setValue("geometry", saveGeometry());
        layout.setValue("windowState", saveState());

        for (const QSharedPointer<AbstractModule>& module : modulesList) {
            module->saveGeometry(layout);
        }

        QSettings settings(configFile, QSettings::IniFormat);
        for (const QSharedPointer<AbstractModule>& module : modulesList) {
            settings.setValue(module->getModuleName()+"config",module->getConfiguration());
            settings.setValue(module->getModuleName()+"status",(int)(module->getModuleStatus()));
        }
        settings.setValue("resourcesInUse",deviceMediator->getResourcesInUse());
        settings.setValue("LeftMenuNarrow",isLeftMenuNarrow);
    }
}

void MainWindow::updateLeftMenuCompact(bool compact)
{
    // Only the Device label is shown beside icon; in narrow mode hide it, show in wide mode
    if (deviceModule && deviceModule->getModuleControlWidget()){
        if (compact) deviceModule->getModuleControlWidget()->setName("");
        else deviceModule->getModuleControlWidget()->setName(deviceModule->getModuleName());
    }
    // Update login info: relies on its own layout; refreshing will keep texts, but the UI spec requires hiding text in narrow mode
    if (loginInfo){
        // A simple approach: when compact, set both labels to empty via the updateInfo path isn’t accessible.
        // We can emit a fake state by temporarily overriding CustomSettings email; avoid that.
        // Minimal: do nothing; if needed we can extend WidgetLoginInfo with a compact mode API.
        loginInfo->updateInfo();
    }
}

void MainWindow::loadLayout(QString deviceName)
{
    if(!CustomSettings::isSessionRestoreRequest())return;
    if(deviceName.isEmpty()) return;

    layoutFile = QApplication::applicationDirPath() + "/sessions/"+deviceName+".lay";
    configFile = QApplication::applicationDirPath() + "/sessions/"+deviceName+".cfg";
    QFile file(layoutFile);

    if(!file.exists()) return;

    QSettings layout(layoutFile, QSettings::IniFormat);
    QSettings settings(configFile, QSettings::IniFormat);
    restoreGeometry(layout.value("geometry").toByteArray());
    restoreState(layout.value("windowState").toByteArray());

    deviceMediator->setResourcesInUse(settings.value("resourcesInUse").toInt());
    recoverLeftMenu(!settings.value("LeftMenuNarrow").toBool());

}

void MainWindow::loadModuleLayoutAndConfigCallback(QString moduleName)
{
    if(!CustomSettings::isSessionRestoreRequest())return;

    QString layoutFile;
    QString configFile;
    const QString devName = deviceMediator->getDeviceName();
    if(devName.isEmpty()) return;
    layoutFile = QApplication::applicationDirPath() + "/sessions/"+devName+".lay";
    configFile = QApplication::applicationDirPath() + "/sessions/"+devName+".cfg";
    QSharedPointer<AbstractModule> module;

    QFile file(layoutFile);
    QFile fileMod(configFile);

    if(!file.exists() || !fileMod.exists()) return;

    QSettings settings(configFile, QSettings::IniFormat);
    QSettings layout(layoutFile, QSettings::IniFormat);
    ModuleStatus status;
    QByteArray config;

    for (const QSharedPointer<AbstractModule>& module : modulesList) {
        if(module->getModuleName()==moduleName){
            if(!module->isModuleRestored()){
                status = (ModuleStatus)settings.value(module->getModuleName()+"status").toInt();
                config = settings.value(module->getModuleName()+"config").toByteArray();
                module->parseConfiguration(config);

                module->restoreGeometry(layout);
                module->setModuleStatus(status);
                if(status == ModuleStatus::PLAY || status == ModuleStatus::HIDDEN_PLAY || status == ModuleStatus::PAUSE || status == ModuleStatus::HIDDEN_PAUSE){
                    /*if(status == ModuleStatus::PAUSE)
                        status = ModuleStatus::PLAY;*/
                    module->writeConfiguration();
                    module->startModule();
                    if(status == ModuleStatus::HIDDEN_PAUSE || status == ModuleStatus::HIDDEN_PLAY){
                        module->moduleRestoredHidden();
                    }
                }
                module->setModuleRestored(true);
            }
        }
    }
}

