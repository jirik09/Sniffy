#include "abstractmodule.h"

AbstractModule::AbstractModule(QObject *parent) : QObject(parent)
{

}

void AbstractModule::widgetControlClicked(ModuleStatus status){
    switch (status) {
    case ModuleStatus::STOP:
        dockWidgetWindow->show();
        writeConfiguration();
        startModule();
        moduleControlWidget->setStatus(ModuleStatus::PLAY);
        break;
    case ModuleStatus::PLAY:
        dockWidgetWindow->hide();
        moduleControlWidget->setStatus(ModuleStatus::HIDDEN_PLAY);
        break;
    case ModuleStatus::PAUSE:
        dockWidgetWindow->hide();
        moduleControlWidget->setStatus(ModuleStatus::HIDDEN_PAUSE);
        break;
    case ModuleStatus::HIDDEN_PLAY:
        dockWidgetWindow->show();
        moduleControlWidget->setStatus(ModuleStatus::PLAY);
        break;
    case ModuleStatus::HIDDEN_PAUSE:
        dockWidgetWindow->show();
        moduleControlWidget->setStatus(ModuleStatus::PAUSE);
        break;
    }
}

void AbstractModule::setDockWidgetWindow(ModuleDockWidget *dockWidget){
    dockWidgetWindow = dockWidget;

    connect(dockWidgetWindow, &ModuleDockWidget::moduleWindowClosing,this, &AbstractModule::closeModule);
}

void AbstractModule::setModuleControlWidget(WidgetControlModule *controlWidget){
    moduleControlWidget = controlWidget;
    connect(moduleControlWidget,SIGNAL(clicked(ModuleStatus)),this,SLOT(widgetControlClicked(ModuleStatus)));
}

void AbstractModule::setCommandPrefix(QByteArray prefix){
    moduleCommandPrefix = prefix;
}

void AbstractModule::setComms(Comms *communication){
    comm = communication;
    cmd = new Commands();
    comm->write(moduleCommandPrefix+":"+Commands::CONFIG_REQUEST+";");
}

void AbstractModule::closeModule(){
    dockWidgetWindow->hide();
    stopModule();
    moduleControlWidget->setStatus(ModuleStatus::STOP);
}

void AbstractModule::disableModule(){
    moduleControlWidget->hide();
    closeModule();
}
