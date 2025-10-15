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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDir>
#include <QFile>


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));

    // The left panel is the central widget; keep it from stretching horizontally
    ui->centralwidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->widget_3->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"logo_gray_full.png);");
    const auto &pal = Graphics::palette();
    ui->centralwidget->setStyleSheet("QWidget{background-color:"+pal.windowWidget+";}"
                                     "QWidget#centralwidget{border-right: 1px solid"+pal.textLabel+";}");

    setWindowTitle("LEO sniffy");
    sett = new SettingsDialog(this);
    connect(sett, &SettingsDialog::saveSessionRequested, this, &MainWindow::onSettingsSaveSessionRequested);
    connect(sett, &SettingsDialog::loadSessionRequested, this, &MainWindow::onSettingsLoadSessionRequested);
    logindial = new LoginDialog(this);

    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    //ui->widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->verticalLayout_modules->addWidget(sep);

    createModulesWidgets();
    setupMainWindowComponents();

    connect(deviceMediator,&DeviceMediator::loadLayoutUponOpen,this,&MainWindow::onOpenLoadSessionRequested,Qt::DirectConnection);
    connect(deviceMediator,&DeviceMediator::saveLayoutUponExit,this,&MainWindow::onExitSaveSessionRequested,Qt::DirectConnection);

    // Start in wide mode by default, then enforce the fixed width
    setMenuWide();
    enforceLeftMenuWidth();
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
        // Whenever a dock changes state/visibility, re-enforce left menu width
        connect(dockWidgets[index], &QDockWidget::visibilityChanged, this, [this](bool){ enforceLeftMenuWidthSoon(); });
        connect(dockWidgets[index], &QDockWidget::topLevelChanged,  this, [this](bool){ enforceLeftMenuWidthSoon(); });
        connect(dockWidgets[index], &QDockWidget::dockLocationChanged, this, [this](Qt::DockWidgetArea){ enforceLeftMenuWidthSoon(); });
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
    connect(logindial,&LoginDialog::loginInfoChanged,deviceMediator,&DeviceMediator::reopenDeviceAfterLogin);

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
    ui->centralwidget->setFixedWidth(LeftMenuWideWidth);
    ui->centralwidget->setMinimumHeight(200);
    ui->centralwidget->setMaximumHeight(20000);
    ui->centralwidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    isLeftMenuNarrow = false;
    updateLeftMenuCompact(false);
    if (loginInfo) loginInfo->setCompact(false);
    enforceLeftMenuWidth();
}

void MainWindow::recoverLeftMenu(bool isWide)
{
    footer->setAppearance(isWide);
    setMenuSize(isWide);
}

void MainWindow::setMenuNarrow(){
    ui->centralwidget->setFixedWidth(LeftMenuNarrowWidth);
    ui->centralwidget->setMinimumHeight(200);
    ui->centralwidget->setMaximumHeight(20000);
    ui->centralwidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    isLeftMenuNarrow = true;
    updateLeftMenuCompact(true);
    if (loginInfo) loginInfo->setCompact(true);
    enforceLeftMenuWidth();
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

void MainWindow::enforceLeftMenuWidth()
{
    // Ensure the left panel width strictly matches the current mode
    const int target = isLeftMenuNarrow ? LeftMenuNarrowWidth : LeftMenuWideWidth;
    if (ui && ui->centralwidget) {
        ui->centralwidget->setFixedWidth(target);
        // Keep horizontal policy fixed to prevent auto-resize
        ui->centralwidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

/** The “soon” version defers the width lock to the next event loop tick so it runs
 *  after QMainWindow finishes its own layout changes triggered by dock events. Dock 
 *  signals fire while QMainWindow is still rearranging docks and doing a layout pass. 
 *  If we lock the width immediately, the subsequent layout can override it.
 *  QTimer::singleShot(0, ...) posts the enforce call to run right after the current event 
 *  processing finishes, i.e., after the dock transition and layout settle.*/
void MainWindow::enforceLeftMenuWidthSoon()
{
    // Defer a tick to let QMainWindow finish its layout pass, then enforce
    QTimer::singleShot(0, this, [this]{ enforceLeftMenuWidth(); });
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    enforceLeftMenuWidth();
}

void MainWindow::onSettingsSaveSessionRequested()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save session", QDir::homePath(), "Session files (*.json)");
    if(fileName.isEmpty()) return;
    saveSessionToFile(fileName, false);
}

void MainWindow::onExitSaveSessionRequested()
{
    if (deviceMediator->getIsConnected())
    {
        QString devName = deviceMediator->getDeviceName();
        if (!devName.isEmpty())
        {
            // Ensure sessions directory exists before attempting to save
            const QString sessionsPath = QApplication::applicationDirPath() + "/sessions";
            QDir sessionsDir(sessionsPath);
            if (!sessionsDir.exists()) {
                // Try to create the sessions directory; if it fails, skip saving
                if (!QDir().mkpath(sessionsPath)) {
                    return;
                }
            }

            const QString fileName = sessionsPath + "/" + devName + ".json";
            saveSessionToFile(fileName,true);
        }
    }
}

void MainWindow::saveSessionToFile(const QString &filePath, bool silent = false)
{
    // Build JSON with layout (geometry, windowState, widget geometries) and config (module configs/status)
    QJsonObject root;

    // Geometry and window state as base64 strings
    root["geometry"] = QString(saveGeometry().toBase64());
    root["windowState"] = QString(saveState().toBase64());

    // Collect layout entries in-memory so we avoid temporary files
    QMap<QString, QByteArray> layoutMap;
    for (const QSharedPointer<AbstractModule>& module : modulesList) {
        module->saveGeometry(layoutMap);
    }

    // Serialize layout map to JSON (base64 encoded values)
    QJsonObject layoutObj;
    for (auto it = layoutMap.constBegin(); it != layoutMap.constEnd(); ++it) {
        layoutObj.insert(it.key(), QString(it.value().toBase64()));
    }
    root["layout"] = layoutObj;

    // Collect module configurations and status
    QJsonArray modulesArr;
    for (const QSharedPointer<AbstractModule>& module : modulesList) {
        QJsonObject mo;
        mo["name"] = module->getModuleName();
        QByteArray cfg = module->getConfiguration();
        mo["config"] = QString(cfg.toBase64());
        mo["status"] = (int)module->getModuleStatus();
        modulesArr.append(mo);
    }
    root["modules"] = modulesArr;

    // Other app state
    root["resourcesInUse"] = deviceMediator->getResourcesInUse().toJson();
    root["LeftMenuNarrow"] = isLeftMenuNarrow;

    // Write JSON to file
    QJsonDocument doc(root);
    QFile out(filePath);
    if(!out.open(QIODevice::WriteOnly)){
        if(!silent){
        QMessageBox::warning(this, "Save session", "Cannot open file for writing");
        }
        return;
    }
    out.write(doc.toJson(QJsonDocument::Indented));
    out.close();
    if(!silent){
        QMessageBox::information(this, "Save session", "Session saved successfully.");
    }
}

void MainWindow::onSettingsLoadSessionRequested()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load session", QDir::homePath(), "Session files (*.json)");
    if(fileName.isEmpty()) return;
    loadSessionFromFile(fileName,true);
}

void MainWindow::onOpenLoadSessionRequested(QString deviceName)
{
    if(deviceName.isEmpty()) return;
    QString fileName = QApplication::applicationDirPath() + "/sessions/"+deviceName+".json";
    QFile file(fileName);
    if(!file.exists()) return;
    loadSessionFromFile(fileName,true);
}

void MainWindow::loadSessionFromFile(const QString &filePath, bool silent = false)
{
    QFile in(filePath);
    if(!in.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "Load session", "Cannot open file for reading");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(in.readAll());
    in.close();
    if(!doc.isObject()){
        QMessageBox::warning(this, "Load session", "Invalid session file");
        return;
    }
    QJsonObject root = doc.object();

    // Restore geometry and window state
    if(root.contains("geometry")){
        QByteArray geom = QByteArray::fromBase64(root.value("geometry").toString().toUtf8());
        restoreGeometry(geom);
    }
    if(root.contains("windowState")){
        QByteArray ws = QByteArray::fromBase64(root.value("windowState").toString().toUtf8());
        restoreState(ws);
    }

    // Reconstruct layout entries in-memory from layout object so we don't need
    // to create a temporary INI file. Keys are moduleName+objectName -> QByteArray
    QMap<QString, QByteArray> layoutMap;
    if(root.contains("layout") && root.value("layout").isObject()){
        QJsonObject layoutObj = root.value("layout").toObject();
        for (auto it = layoutObj.begin(); it != layoutObj.end(); ++it){
            QByteArray b = QByteArray::fromBase64(it.value().toString().toUtf8());
            layoutMap.insert(it.key(), b);
        }
    }

    // Apply module configurations
    if(root.contains("modules") && root.value("modules").isArray()){
        QJsonArray modulesArr = root.value("modules").toArray();
        for (const QJsonValue &mv : modulesArr){
            if(!mv.isObject()) continue;
            QJsonObject mo = mv.toObject();
            QString name = mo.value("name").toString();
            QByteArray cfg = QByteArray::fromBase64(mo.value("config").toString().toUtf8());
            ModuleStatus status = (ModuleStatus)mo.value("status").toInt();

            for (const QSharedPointer<AbstractModule>& module : modulesList) {
                if(module->getModuleName() == name){
                    // Apply configuration
                    module->parseConfiguration(cfg);
                    // Restore per-module geometry from in-memory map
                    module->restoreGeometry(layoutMap);
                    // Set status
                    module->setModuleStatus((ModuleStatus)status);
                    module->setModuleRestored(true);
                    // If module should be running, ensure it starts
                    if(status == ModuleStatus::PLAY || status == ModuleStatus::HIDDEN_PLAY || status == ModuleStatus::PAUSE || status == ModuleStatus::HIDDEN_PAUSE){
                        module->writeConfiguration();
                        module->startModule();
                        if(status == ModuleStatus::HIDDEN_PAUSE || status == ModuleStatus::HIDDEN_PLAY){
                            module->moduleRestoredHidden();
                        }
                        ResourceSet starterSet = ResourceSet::fromModule(module, module->getResources());
                        qDebug() << "Loading session, reserving resources for " << name << " res=" << QString::number(starterSet.resources,16)<<" gpioA="<<QString::number(starterSet.gpioA,16)<<" gpioB="<<QString::number(starterSet.gpioB,16)<<" gpioC="<<QString::number(starterSet.gpioC,16)<<" gpioD="<<QString::number(starterSet.gpioD,16);
                        deviceMediator->setResourcesInUse(starterSet);
                    }
                    break;
                }
            }
        }
    }

    // Resources and left menu
    if(root.contains("resourcesInUse")){
        qDebug() << "Restoring resources in use from session";
        deviceMediator->setResourcesInUse(ResourceSet::fromJson(root.value("resourcesInUse").toObject()));
    }
    if(root.contains("LeftMenuNarrow")){
        bool leftN = root.value("LeftMenuNarrow").toBool();
        recoverLeftMenu(!leftN);
    }
    if(!silent){
        QMessageBox::information(this, "Load session", "Session loaded successfully.");
    }
}

