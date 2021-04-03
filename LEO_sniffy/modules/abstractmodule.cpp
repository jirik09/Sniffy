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
    QList<WidgetSelection*> listSel = getWidget()->findChildren<WidgetSelection*>();
    WidgetSelection *sel;
    QList<WidgetTextInput*> listText = getWidget()->findChildren<WidgetTextInput*>();
    WidgetTextInput *text;

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
    foreach(sel, listSel){
        if (!sel->objectName().isEmpty()){
            layout.setValue(moduleName+sel->objectName(),sel->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: selection in "<<moduleName;
        }
    }
    foreach(text, listText){
        if (!text->objectName().isEmpty()){
            layout.setValue(moduleName+text->objectName(),text->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Tect input in "<<moduleName;
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
    QList<WidgetSelection*> listSel = getWidget()->findChildren<WidgetSelection*>();
    WidgetSelection *sel;
    QList<WidgetTextInput*> listText = getWidget()->findChildren<WidgetTextInput*>();
    WidgetTextInput *text;

    foreach(tab, listTab){
        if (!tab->objectName().isEmpty() && !layout.value(moduleName+tab->objectName()).isNull()){
            tab->restoreGeometry(layout.value(moduleName+tab->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: tab in "<<moduleName;
        }
    }
    foreach(btn, listBtn){
        if (!btn->objectName().isEmpty() && !layout.value(moduleName+btn->objectName()).isNull()){
            btn->restoreGeometry(layout.value(moduleName+btn->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: button in "<<moduleName;
        }
    }
    foreach(sw, listSw){
        if (!sw->objectName().isEmpty() && !layout.value(moduleName+sw->objectName()).isNull()){
            sw->restoreGeometry(layout.value(moduleName+sw->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: switch in "<<moduleName;
        }
    }
    foreach(dial, listDial){
        if (!dial->objectName().isEmpty() && !layout.value(moduleName+dial->objectName()).isNull()){
            dial->restoreGeometry(layout.value(moduleName+dial->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: dial in "<<moduleName;
        }
    }
    foreach(dialRange, listDialRange){
        if (!dialRange->objectName().isEmpty() && !layout.value(moduleName+dialRange->objectName()).isNull()){
             dialRange->restoreGeometry(layout.value(moduleName+dialRange->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: dialRange in "<<moduleName;
        }
    }
    foreach(sel, listSel){
        if (!sel->objectName().isEmpty() && !layout.value(moduleName+sel->objectName()).isNull()){
             sel->restoreGeometry(layout.value(moduleName+sel->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: selection in "<<moduleName;
        }
    }
    foreach(text, listText){
        if (!text->objectName().isEmpty() && !layout.value(moduleName+text->objectName()).isNull()){
             text->restoreGeometry(layout.value(moduleName+text->objectName()).toByteArray());
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: textInput in "<<moduleName;
        }
    }
}


void AbstractModule::widgetControlClicked(ModuleStatus status){
    switch (status) {
    case ModuleStatus::STOP:
        emit blockConflictingModules(moduleName, moduleSpecification->getResources());
        dockWidgetWindow->show();
        writeConfiguration();
        moduleControlWidget->setStatus(ModuleStatus::PLAY);
        startModule();
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
    case ModuleStatus::LOCKED:
        //TODO decide whether to close used resources or not open
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
    if(moduleControlWidget->getStatus()!=ModuleStatus::STOP)
        stopModule();
    moduleControlWidget->setStatus(ModuleStatus::STOP);
    if(moduleSpecification != nullptr)
        emit releaseConflictingModules(moduleName, moduleSpecification->getResources());
}

void AbstractModule::disableModule(){
    moduleControlWidget->hide();
    closeModule();
}

bool AbstractModule::isActive()
{
    if (moduleControlWidget->getStatus()==ModuleStatus::STOP || moduleControlWidget->getStatus()==ModuleStatus::LOCKED){
        return false;
    }
    return true;
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

int AbstractModule::getResources()
{
    if(moduleSpecification != nullptr)
        return moduleSpecification->getResources();
    return 0;
}

bool AbstractModule::isModuleRestored() const
{
    return moduleRestored;
}

void AbstractModule::setModuleRestored(bool value)
{
    moduleRestored = value;
}

void AbstractModule::held(bool held){
    emit holdClicked(held);
}

