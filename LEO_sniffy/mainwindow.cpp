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

    connect(deviceMediator,&DeviceMediator::loadLayout,this,&MainWindow::loadLayout);
}

void MainWindow::createModulesWidgets(){
    deviceMediator = new DeviceMediator(this);

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

void MainWindow::closeEvent (QCloseEvent *event)
{
    saveLayout();
    event->accept();
    /*QMessageBox::StandardButton resBtn = QMessageBox::question( this, "APP_NAME",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }*/
}

void MainWindow::saveLayout()
{
    if(deviceMediator->getIsConnected()){
        layoutFile = QApplication::applicationDirPath() + "/"+deviceMediator->getDeviceName()+".lay";
        moduleFile = QApplication::applicationDirPath() + "/"+deviceMediator->getDeviceName()+".mod";

        QFile file(layoutFile);
        if (!file.open(QFile::WriteOnly)) {
            QString msg = tr("Failed to open %1\n%2")
                    .arg(QDir::toNativeSeparators(layoutFile), file.errorString());
            QMessageBox::warning(this, tr("Error"), msg);
            return;
        }

        QByteArray geo_data = saveGeometry();
        QByteArray layout_data = saveState();

        bool ok = file.putChar((uchar)geo_data.size());
        if (ok)
            ok = file.write(geo_data) == geo_data.size();
        if (ok)
            ok = file.write(layout_data) == layout_data.size();

        if (!ok) {
            QString msg = tr("Error writing to %1\n%2")
                    .arg(QDir::toNativeSeparators(layoutFile), file.errorString());
            QMessageBox::warning(this, tr("Error"), msg);
            return;
        }

        file.close();

        QSettings settings(moduleFile, QSettings::IniFormat);
        QSharedPointer<AbstractModule> module;
        foreach(module, modulesList){
            settings.setValue(module->getModuleName(),(int)(module->getModuleStatus()));
        }
    }
}

void MainWindow::loadLayout(QString deviceName)
{
    layoutFile = QApplication::applicationDirPath() + "/"+deviceName+".lay";
    moduleFile = QApplication::applicationDirPath() + "/"+deviceName+".mod";

    QFile file(layoutFile);
    QFile fileMod(moduleFile);

    if(!file.exists() || !fileMod.exists()){
        return;
    }

    if (!file.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                .arg(QDir::toNativeSeparators(layoutFile), file.errorString());
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }

    uchar geo_size;
    QByteArray geo_data;
    QByteArray layout_data;

    bool ok = file.getChar((char*)&geo_size);
    if (ok) {
        geo_data = file.read(geo_size);
        ok = geo_data.size() == geo_size;
    }
    if (ok) {
        layout_data = file.readAll();
        ok = layout_data.size() > 0;
    }

    if (ok)
        ok = restoreGeometry(geo_data);
    if (ok)
        ok = restoreState(layout_data);

    if (!ok) {
        QString msg = tr("Error reading %1").arg(QDir::toNativeSeparators(layoutFile));
        QMessageBox::warning(this, tr("Error"), msg);
        return;
    }


    QSettings settings(moduleFile, QSettings::IniFormat);
    QSharedPointer<AbstractModule> module;
    ModuleStatus status;
    foreach(module, modulesList){
        status = (ModuleStatus)settings.value(module->getModuleName()).toInt();
        if(status == ModuleStatus::PLAY || status == ModuleStatus::HIDDEN_PLAY){
            module->startModule();
        }
        module->setModuleStatus(status);
    }
}


