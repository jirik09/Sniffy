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
#include <QFileInfo>
#include <QToolTip>
#include <QScreen>
#include <QGuiApplication>


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));

    // Before anything shows, optionally restore only position/size from the newest session
    if (CustomSettings::getSmartSessionLayoutGeometry()) {
        restoreInitialGeometryFromNewestSession();
    }

    // The left panel is the central widget; keep it from stretching horizontally
    ui->centralwidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->widget_3->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"logo_gray_full.png);");
    const auto &pal = Graphics::palette();
    ui->centralwidget->setStyleSheet("QWidget{background-color:"+pal.windowWidget+";}"
                                     "QWidget#centralwidget{border-right: 1px solid"+pal.textLabel+";}");

    setWindowTitle("LEO sniffy");
    
    // Create shared authenticator
    authenticator = new Authenticator(this);

    sett = new SettingsDialog(authenticator, this);
    connect(sett, &SettingsDialog::saveSessionRequested, this, &MainWindow::onSettingsSaveSessionRequested);
    connect(sett, &SettingsDialog::loadSessionRequested, this, &MainWindow::onSettingsLoadSessionRequested);
    
    logindial = new LoginDialog(authenticator, this);
    connect(logindial, &LoginDialog::loginInfoChangedReopen, sett, &SettingsDialog::onUserLoginChanged);

    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    //ui->widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->verticalLayout_modules->addWidget(sep);

    createModulesWidgets();
    setupMainWindowComponents();

    connect(sett, &SettingsDialog::firmwareFlashed, deviceMediator, &DeviceMediator::reopenDeviceAfterLogin);
    connect(deviceMediator,&DeviceMediator::loadLayoutUponOpen,this,&MainWindow::onOpenLoadSessionRequested,Qt::DirectConnection);
    connect(deviceMediator,&DeviceMediator::saveLayoutUponExit,this,&MainWindow::onExitSaveSessionRequested,Qt::DirectConnection);

    // Start in wide mode by default, then enforce the fixed width
    setMenuWide();
    enforceLeftMenuWidth();
}

void MainWindow::restoreInitialGeometryFromNewestSession()
{
    const QString sessionsPath = QApplication::applicationDirPath() + "/sessions";
    QDir dir(sessionsPath);
    if (!dir.exists()) return;

    // Find newest *.json by modification time (descending)
    const QFileInfoList files = dir.entryInfoList({"*.json"}, QDir::Files, QDir::Time);
    if (files.isEmpty()) return;

    const QString newest = files.first().absoluteFilePath();
    QFile in(newest);
    if (!in.open(QIODevice::ReadOnly)) return;
    const QJsonDocument doc = QJsonDocument::fromJson(in.readAll());
    in.close();
    if (!doc.isObject()) return;

    const QJsonObject obj = doc.object();
    const auto geomVal = obj.value("geometry");
    if (geomVal.isString()){
        const QByteArray geom = QByteArray::fromBase64(geomVal.toString().toUtf8());
        if (!geom.isEmpty()){
            restoreGeometry(geom);
        }
    }
}

void MainWindow::applySmartSessionSizeOnly()
{
    if (!CustomSettings::getSmartSessionLayoutGeometry()) return;
    if (isMaximized() || isFullScreen()) return;
    if (!sessionRestoreData.contains("geometry")) return;

    const QByteArray geomBytes = QByteArray::fromBase64(sessionRestoreData.value("geometry").toString().toUtf8());
    if (geomBytes.isEmpty()) return;

    // Use a temporary window to decode saved size without moving this window
    QMainWindow tmp;
    tmp.restoreGeometry(geomBytes);
    QSize target = tmp.size();
    if (!target.isValid() || target.isEmpty()) return;

    // Fit to available screen while expanding symmetrically around current frame center
    QScreen *scr = this->screen();
    if (!scr) scr = QGuiApplication::primaryScreen();
    if (!scr) return;
    const QRect avail = scr->availableGeometry();

    const QRect fr = frameGeometry();
    QPoint center = fr.center();

    // Calculate frame overhead to convert target content size to frame size
    const QSize frameToWidgetDelta = fr.size() - geometry().size();
    const QSize targetFrameSize = target + frameToWidgetDelta;

    // Desired frame rect centered on current center
    QRect desiredFR(QPoint(0,0), targetFrameSize);
    desiredFR.moveCenter(center);

    // Clamp size if larger than available area
    if (desiredFR.width() > avail.width()) desiredFR.setWidth(avail.width());
    if (desiredFR.height() > avail.height()) desiredFR.setHeight(avail.height());

    // Re-center after size clamp
    desiredFR.moveCenter(center);

    // Shift inside available area (minimal movement)
    if (desiredFR.left() < avail.left()) desiredFR.moveLeft(avail.left());
    if (desiredFR.right() > avail.right()) desiredFR.moveRight(avail.right());
    if (desiredFR.top() < avail.top()) desiredFR.moveTop(avail.top());
    if (desiredFR.bottom() > avail.bottom()) desiredFR.moveBottom(avail.bottom());

    // Convert frame rect to widget geometry rect
    const QPoint frameToWidgetOffset = fr.topLeft() - geometry().topLeft();
    // frameToWidgetDelta is already defined above
    const QPoint widgetTopLeft = desiredFR.topLeft() - frameToWidgetOffset;
    const QSize widgetSize = desiredFR.size() - frameToWidgetDelta;

    // Guard: ensure non-negative size
    QSize finalSize(qMax(widgetSize.width(), minimumSize().width()), qMax(widgetSize.height(), minimumSize().height()));
    setGeometry(QRect(widgetTopLeft, finalSize));
}

void MainWindow::createModulesWidgets(){
    deviceMediator = new DeviceMediator(authenticator, this);

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

        // Connect signal for deferred session restoration when module shows its control
    connect(module.data(), &AbstractModule::moduleControlShown, this, &MainWindow::loadModulesSessionFromFile);



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
    connect(logindial,&LoginDialog::loginInfoChangedReopen,deviceMediator,&DeviceMediator::reopenDeviceAfterLogin);
    connect(logindial,&LoginDialog::loginInfoChangedReopen,this,&MainWindow::onLoginInfoChanged);
    connect(authenticator, &Authenticator::authenticationSucceeded, this, &MainWindow::updateLoginInfo);
    connect(authenticator, &Authenticator::popupMessageRequested, this, &MainWindow::showBottomLeftPopup);

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

void MainWindow::updateLoginInfo(const QDateTime &validity, const QByteArray &token, bool forceReconnect)
{
    Q_UNUSED(validity);
    Q_UNUSED(token);
    Q_UNUSED(forceReconnect);
    loginInfo->updateInfo();
}

void MainWindow::onLoginInfoChanged()
{
    loginInfo->updateInfo();
}

void MainWindow::openSettingDialog()
{
    sett->open();
}

void MainWindow::showBottomLeftPopup(const QString &text)
{
    QTimer::singleShot(0, this, [this, text] {
        const int margin = 14;   // distance from edges
        const int lift   = 36;   // raise a bit so it doesn't clip at the bottom
        QPoint local(margin, this->height() - margin - lift);
        QPoint global = this->mapToGlobal(local);
        QFont tipFont = QApplication::font();
        tipFont.setPointSize(10);
        QToolTip::setFont(tipFont);
        QToolTip::showText(global, text, this, QRect(), 10000);
    });
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
        if (sett) sett->closeDialog(true);
        showBottomLeftPopup("Session saved successfully");
    }
}

bool MainWindow::loadSessionJSONFile(const QString &filePath)
{
    QFile in(filePath);
    if(!in.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "Load session", "Cannot open file for reading");
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(in.readAll());
    in.close();
    if(!doc.isObject()){
        QMessageBox::warning(this, "Load session", "Invalid session file");
        return false;
    }
    
    sessionRestoreData = doc.object();
    return true;
}

void MainWindow::onSettingsLoadSessionRequested()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load session", QDir::homePath(), "Session files (*.json)");
    if(fileName.isEmpty()) return;
    
    // Close all modules before loading new session
    deviceMediator->closeModules();
    
    if(!loadSessionJSONFile(fileName)) return;
    
    // If smart session is enabled, don't move the main window when user loads a session
    {
        const bool smart = CustomSettings::getSmartSessionLayoutGeometry();
        loadLayoutSessionFromFile(!smart ? true : false);
        if (smart) {
            // Keep position, update size only (symmetrically, clamped to screen)
            applySmartSessionSizeOnly();
        }
    }
    for (const QSharedPointer<AbstractModule>& module : modulesList) {
        loadModulesSessionFromFile(module->getModuleName());
    }
    if (sett) sett->closeDialog(true);
    showBottomLeftPopup("Session loaded successfully");
    //QMessageBox::information(this, "Load session", "Session loaded successfully.");
}

void MainWindow::onOpenLoadSessionRequested(QString deviceName)
{
    if(deviceName.isEmpty()) return;
    
    QString fileName;
    if(deviceName == "layoutOnly") {
        // Load default layout from embedded resource
        fileName = ":/default_layout.json";
    } else {
        // Load device-specific session
        fileName = QApplication::applicationDirPath() + "/sessions/" + deviceName + ".json";
    }
    
    QFile file(fileName);
    if(!file.exists()) {
        return;
    }
    
    if(!loadSessionJSONFile(fileName)) {
        return;
    }
    
    if(deviceName == "layoutOnly") {
        if(sessionRestoreData.contains("windowState")){
            QByteArray ws = QByteArray::fromBase64(sessionRestoreData.value("windowState").toString().toUtf8());
            restoreState(ws);
        }   
        sessionRestoreData = QJsonObject();
    }else{
        // If smart session is enabled, we already restored initial geometry on startup; avoid moving the window again
        const bool smart = CustomSettings::getSmartSessionLayoutGeometry();
        loadLayoutSessionFromFile(!smart ? true : false);
        if (smart) {
            // Keep position, update size only (symmetrically, clamped to screen)
            applySmartSessionSizeOnly();
        }
    }
}

void MainWindow::loadLayoutSessionFromFile(bool restoreMainGeometry)
{
    // Restore geometry and window state
    if(restoreMainGeometry && sessionRestoreData.contains("geometry")){
        QByteArray geom = QByteArray::fromBase64(sessionRestoreData.value("geometry").toString().toUtf8());
    restoreGeometry(geom);
    }
    if(sessionRestoreData.contains("windowState")){
        QByteArray ws = QByteArray::fromBase64(sessionRestoreData.value("windowState").toString().toUtf8());
        restoreState(ws);
    }

    // Load layout map from JSON
    QMap<QString, QByteArray> layoutMap;
    if(sessionRestoreData.contains("layout") && sessionRestoreData.value("layout").isObject()){
        QJsonObject layoutObj = sessionRestoreData.value("layout").toObject();
        for (auto it = layoutObj.begin(); it != layoutObj.end(); ++it){
            QByteArray b = QByteArray::fromBase64(it.value().toString().toUtf8());
            layoutMap.insert(it.key(), b);
        }
    }

    // Restore per-module geometry from in-memory map (but not config yet)
    for (const QSharedPointer<AbstractModule>& module : modulesList) {
        module->restoreGeometry(layoutMap);
    }

    if(sessionRestoreData.contains("LeftMenuNarrow")){
        bool leftN = sessionRestoreData.value("LeftMenuNarrow").toBool();
        recoverLeftMenu(!leftN);
    }
}

void MainWindow::loadModulesSessionFromFile(const QString &moduleName)
{
    // Apply module configurations for the specified module
    if(sessionRestoreData.contains("modules") && sessionRestoreData.value("modules").isArray()){
        QJsonArray modulesArr = sessionRestoreData.value("modules").toArray();
        for (const QJsonValue &mv : modulesArr){
            if(!mv.isObject()) continue;
            QJsonObject mo = mv.toObject();
            QString name = mo.value("name").toString();
            
            // Only process the module that just showed its control
            if(name != moduleName) continue;
            
            QByteArray cfg = QByteArray::fromBase64(mo.value("config").toString().toUtf8());
            ModuleStatus status = (ModuleStatus)mo.value("status").toInt();

            for (const QSharedPointer<AbstractModule>& module : modulesList) {
                if(module->getModuleName() == name){
                    // Apply configuration
                    module->parseConfiguration(cfg);
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
                        deviceMediator->setResourcesInUse(starterSet);
                    }
                    break;
                }
            }
            break; // Found and processed the module, exit loop
        }
    }
}

