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

//#include "GUI/moduledockwidget.h"
//#include "GUI/widgetcontrolmodule.h"
//#include "GUI/widgetseparator.h"
//#include "modules/scope/windowscope.h"
//#include "modules/counter/windowcounter.h"
//#include "GUI/widgettab.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    device = new DeviceMediator(this);

    WidgetSeparator *sep = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sep);

    /* Build MainWindow-related parts of Moduls */   
    QList<QSharedPointer<AbstractModule>> modulesList = device->getModulesList();
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

    /* Show DeviceWindow straight away */
    dockWidget[0]->show();

    /* Left pane Menu of Control Play/Pause buttons */

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_modules->addItem(verticalSpacer);
    WidgetSeparator *sepa = new WidgetSeparator(ui->centralwidget);
    ui->verticalLayout_modules->addWidget(sepa);
    WidgetFooter *footer = new WidgetFooter();
    ui->verticalLayout_modules->addWidget(footer);

    connect(footer->getPushButtonSize(),SIGNAL(clicked()),this,SLOT(setMenuSize()));

    QVBoxLayout *horizontalLayout;
    horizontalLayout = new QVBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

   // addDockWidget(static_cast<Qt::DockWidgetArea>(2), ScnaWindowtamšopnout);

    //pass scope pointer and pointer to module widget into scope window - this connects mediator to GUI
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMenuSize(){
    if(ui->centralwidget->geometry().width()>150){
       ui->centralwidget->setMinimumSize(100,200);
       ui->centralwidget->setMaximumSize(100,20000);
    }else{
       ui->centralwidget->setMinimumSize(250,200);
       ui->centralwidget->setMaximumSize(250,20000);
    }
}

/* End of File my Friend */
