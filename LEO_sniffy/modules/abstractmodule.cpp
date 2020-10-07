#include "abstractmodule.h"

AbstractModule::AbstractModule(QObject *parent) : QObject(parent)
{

}

void AbstractModule::saveGeometry(QSettings &layout)
{
    QList<WidgetButtons*> listBtn = getWidget()->findChildren<WidgetButtons*>();
    WidgetButtons *btn;
    QList<WidgetDial*> listDial = getWidget()->findChildren<WidgetDial*>();
    WidgetDial *dial;
    QList<WidgetDialRange*> listDialRange = getWidget()->findChildren<WidgetDialRange*>();
    WidgetDialRange *dialRange;
    QList<WidgetSwitch*> listSw = getWidget()->findChildren<WidgetSwitch*>();
    WidgetSwitch *sw;
    QList<widgetTab*> listTab = getWidget()->findChildren<widgetTab*>();
    widgetTab *tab;

    foreach(tab, listTab){
        if (!tab->objectName().isEmpty()){
            layout.setValue(moduleName+tab->objectName(),tab->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Tab in "<<moduleName;
        }
    }
    foreach(btn, listBtn){
        if (!btn->objectName().isEmpty()){
            layout.setValue(moduleName+btn->objectName(),btn->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Button in "<<moduleName;
        }
    }
    foreach(sw, listSw){
        if (!sw->objectName().isEmpty()){
            layout.setValue(moduleName+sw->objectName(),sw->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Switch in "<<moduleName;
        }
    }
    foreach(dial, listDial){
        if (!dial->objectName().isEmpty()){
            layout.setValue(moduleName+dial->objectName(),dial->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Dial in "<<moduleName;
        }
    }
    foreach(dialRange, listDialRange){
        if (!dialRange->objectName().isEmpty()){
            layout.setValue(moduleName+dialRange->objectName(),dialRange->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Dial range in "<<moduleName;
        }
    }
}

void AbstractModule::restoreGeometry(QSettings &layout)
{
    QList<WidgetButtons*> listBtn = getWidget()->findChildren<WidgetButtons*>();
    WidgetButtons *btn;
    QList<WidgetDial*> listDial = getWidget()->findChildren<WidgetDial*>();
    WidgetDial *dial;
    QList<WidgetDialRange*> listDialRange = getWidget()->findChildren<WidgetDialRange*>();
    WidgetDialRange *dialRange;
    QList<WidgetSwitch*> listSw = getWidget()->findChildren<WidgetSwitch*>();
    WidgetSwitch *sw;
    QList<widgetTab*> listTab = getWidget()->findChildren<widgetTab*>();
    widgetTab *tab;

    foreach(tab, listTab){
        if (!tab->objectName().isEmpty()){
            tab->restoreGeometry(layout.value(moduleName+tab->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: tab in "<<moduleName;
        }
    }
    foreach(btn, listBtn){
        if (!btn->objectName().isEmpty()){
            btn->restoreGeometry(layout.value(moduleName+btn->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: button in "<<moduleName;
        }
    }
    foreach(sw, listSw){
        if (!sw->objectName().isEmpty()){
            sw->restoreGeometry(layout.value(moduleName+sw->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: switch in "<<moduleName;
        }
    }
    foreach(dial, listDial){
        if (!dial->objectName().isEmpty()){
            dial->restoreGeometry(layout.value(moduleName+dial->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: dial in "<<moduleName;
        }
    }
    foreach(dialRange, listDialRange){
        if (!dialRange->objectName().isEmpty()){
            dialRange->restoreGeometry(layout.value(moduleName+dialRange->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: dialRange in "<<moduleName;
        }
    }
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

    connect(dockWidgetWindow, &ModuleDockWidget::moduleWindowClosing, this, &AbstractModule::closeModule);
    connect(dockWidgetWindow, &ModuleDockWidget::holdClicked, this, &AbstractModule::held);
}

void AbstractModule::setModuleControlWidget(WidgetControlModule *controlWidget){
    moduleControlWidget = controlWidget;
    connect(moduleControlWidget,SIGNAL(clicked(ModuleStatus)),this,SLOT(widgetControlClicked(ModuleStatus)));

    moduleControlWidget->setIcon(moduleIconURI);
    dockWinCreated = true;
    emit moduleCreated();
}

QByteArray AbstractModule::getCommandPrefix()
{
    return moduleCommandPrefix;
}

void AbstractModule::setComms(Comms *communication){
    comm = communication;
    cmd = new Commands();
    comm->write(moduleCommandPrefix+":"+Commands::CONFIG_REQUEST+";");
}

void AbstractModule::hideModuleStatus(){
    moduleControlWidget->hideStatus();
}

void AbstractModule::setModuleStatus(ModuleStatus stat){
    moduleControlWidget->setStatus(stat);
}

ModuleStatus AbstractModule::getModuleStatus()
{
    return moduleControlWidget->getStatus();
}

void AbstractModule::showModuleControl(){
    emit loadModuleLayoutAndConfig(moduleName);
    moduleControlWidget->show();
}

void AbstractModule::showModuleWindow(){
    dockWidgetWindow->show();
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

void AbstractModule::setIcon (QString ImageURI){
    moduleControlWidget->setIcon(ImageURI);
}

QString AbstractModule::getModuleName()
{
    return moduleName;
}

void AbstractModule::setModuleName(QString value){
    moduleName = value;
    moduleControlWidget->setName(moduleName);
}

/* This function must be called after dockWidget is created.
   Connect signal moduleCreated() in the module... */
void AbstractModule::showModuleHoldButton(){
    if(dockWinCreated)
        dockWidgetWindow->showHoldButton();
}

void AbstractModule::held(bool held){
    emit holdClicked(held);
}

