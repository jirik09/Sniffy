#include "abstractmodule.h"
#include "qstyle.h"

AbstractModule::AbstractModule(QObject *parent) : QObject(parent)
{
}

void AbstractModule::saveGeometry(QMap<QString, QByteArray> &layoutMap)
{
    QList<WidgetButtons*> listBtn = getWidget()->findChildren<WidgetButtons*>();
    QList<WidgetDial*> listDial = getWidget()->findChildren<WidgetDial*>();
    QList<WidgetDialRange*> listDialRange = getWidget()->findChildren<WidgetDialRange*>();
    QList<WidgetSwitch*> listSw = getWidget()->findChildren<WidgetSwitch*>();
    QList<widgetTab*> listTab = getWidget()->findChildren<widgetTab*>();
    QList<WidgetSelection*> listSel = getWidget()->findChildren<WidgetSelection*>();
    QList<WidgetTextInput*> listText = getWidget()->findChildren<WidgetTextInput*>();

    for (widgetTab* tab : listTab){
        if (!tab->objectName().isEmpty()){
            layoutMap.insert(moduleName + tab->objectName(), tab->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Tab in "<<moduleName;
        }
    }
    for (WidgetButtons* btn : listBtn){
        if (!btn->objectName().isEmpty()){
            layoutMap.insert(moduleName + btn->objectName(), btn->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Button in "<<moduleName;
        }
    }
    for (WidgetSwitch* sw : listSw){
        if (!sw->objectName().isEmpty()){
            layoutMap.insert(moduleName + sw->objectName(), sw->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Switch in "<<moduleName;
        }
    }
    for (WidgetDial* dial : listDial){
        if (!dial->objectName().isEmpty()){
            layoutMap.insert(moduleName + dial->objectName(), dial->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Dial in "<<moduleName;
        }
    }
    for (WidgetDialRange* dialRange : listDialRange){
        if (!dialRange->objectName().isEmpty()){
            layoutMap.insert(moduleName + dialRange->objectName(), dialRange->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Dial range in "<<moduleName;
        }
    }
    for (WidgetSelection* sel : listSel){
        if (!sel->objectName().isEmpty()){
            layoutMap.insert(moduleName + sel->objectName(), sel->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: selection in "<<moduleName;
        }
    }
    for (WidgetTextInput* text : listText){
        if (!text->objectName().isEmpty()){
            layoutMap.insert(moduleName + text->objectName(), text->saveGeometry());
        }else{
            qDebug () << "WARNING attempting to save layout of object without name: Tect input in "<<moduleName;
        }
    }
    QList<widgetChart*> listChart = getWidget()->findChildren<widgetChart*>();
    for (int i = 0; i < listChart.size(); i++){
        layoutMap.insert(moduleName + "chart_" + QString::number(i), listChart[i]->saveGeometry());
    }
}

void AbstractModule::restoreGeometry(const QMap<QString, QByteArray> &layoutMap)
{
    QList<WidgetButtons*> listBtn = getWidget()->findChildren<WidgetButtons*>();
    QList<WidgetDial*> listDial = getWidget()->findChildren<WidgetDial*>();
    QList<WidgetDialRange*> listDialRange = getWidget()->findChildren<WidgetDialRange*>();
    QList<WidgetSwitch*> listSw = getWidget()->findChildren<WidgetSwitch*>();
    QList<widgetTab*> listTab = getWidget()->findChildren<widgetTab*>();
    QList<WidgetSelection*> listSel = getWidget()->findChildren<WidgetSelection*>();
    QList<WidgetTextInput*> listText = getWidget()->findChildren<WidgetTextInput*>();

    for (widgetTab* tab : listTab){
        QString key = moduleName + tab->objectName();
        if (!tab->objectName().isEmpty() && layoutMap.contains(key)){
            tab->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: tab in "<<moduleName;
        }
    }
    for (WidgetButtons* btn : listBtn){
        QString key = moduleName + btn->objectName();
        if (!btn->objectName().isEmpty() && layoutMap.contains(key)){
            btn->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: button in "<<moduleName;
        }
    }
    for (WidgetSwitch* sw : listSw){
        QString key = moduleName + sw->objectName();
        if (!sw->objectName().isEmpty() && layoutMap.contains(key)){
            sw->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: switch in "<<moduleName;
        }
    }
    for (WidgetDial* dial : listDial){
        QString key = moduleName + dial->objectName();
        if (!dial->objectName().isEmpty() && layoutMap.contains(key)){
            dial->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: dial in "<<moduleName;
        }
    }
    for (WidgetDialRange* dialRange : listDialRange){
        QString key = moduleName + dialRange->objectName();
        if (!dialRange->objectName().isEmpty() && layoutMap.contains(key)){
            dialRange->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: dialRange in "<<moduleName;
        }
    }
    for (WidgetSelection* sel : listSel){
        QString key = moduleName + sel->objectName();
        if (!sel->objectName().isEmpty() && layoutMap.contains(key)){
            sel->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: selection in "<<moduleName;
        }
    }
    for (WidgetTextInput* text : listText){
        QString key = moduleName + text->objectName();
        if (!text->objectName().isEmpty() && layoutMap.contains(key)){
            text->restoreGeometry(layoutMap.value(key));
        }else{
            qDebug () << "WARNING layout cannot be restored due to missing object name: textInput in "<<moduleName;
        }
    }
    QList<widgetChart*> listChart = getWidget()->findChildren<widgetChart*>();
    for (int i = 0; i < listChart.size(); i++){
        QString key = moduleName + "chart_" + QString::number(i);
        if (layoutMap.contains(key)){
            listChart[i]->restoreGeometry(layoutMap.value(key));
        }
    }
}


void AbstractModule::widgetControlClicked(ModuleStatus status){
    switch (status) {
    case ModuleStatus::STOP:
        emit blockConflictingModules(moduleName, moduleSpecification->getResources());
        dockWidgetWindow->show();
        moduleControlWidget->setStatus(ModuleStatus::PLAY);
        writeConfiguration();
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
    case ModuleStatus::WAIT_EVENT:
        dockWidgetWindow->hide();
        moduleControlWidget->setStatus(ModuleStatus::HIDDEN_WAIT_EVENT);
        break;
    case ModuleStatus::HIDDEN_WAIT_EVENT:
        dockWidgetWindow->show();
        moduleControlWidget->setStatus(ModuleStatus::WAIT_EVENT);
        break;
    case ModuleStatus::LOCKED:
        //TODO decide whether to close used resources or not open
        break;
    }
}

void AbstractModule::widgetHideWindowClicked()
{
    widgetControlClicked(moduleControlWidget->getStatus());
}

void AbstractModule::widgetControlClickedWheel(QMouseEvent *event){
    if(event->button() == Qt::MiddleButton){
        closeModule();
    }
}

void AbstractModule::setDockWidgetWindow(ModuleDockWidget *dockWidget){
    dockWidgetWindow = dockWidget;

    connect(dockWidgetWindow, &ModuleDockWidget::moduleWindowClosing, this, &AbstractModule::closeModule);
    connect(dockWidgetWindow, &ModuleDockWidget::holdClicked, this, &AbstractModule::held);
    connect(dockWidgetWindow, &ModuleDockWidget::hideModuleWindowClicked, this, &AbstractModule::widgetHideWindowClicked);
}

void AbstractModule::setModuleControlWidget(WidgetControlModule *controlWidget){
    moduleControlWidget = controlWidget;
    connect(moduleControlWidget, &WidgetControlModule::clicked,this, &AbstractModule::widgetControlClicked);
    connect(moduleControlWidget, &WidgetControlModule::mouseWheelPressEvent,this, &AbstractModule::widgetControlClickedWheel);

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
    if (!cmd) cmd = new Commands();
    
    // Send CONFIG? request
    comm->write(moduleCommandPrefix+":"+Commands::CONFIG_REQUEST+";");
}

void AbstractModule::hideModuleStatus(){
    moduleControlWidget->hideStatus();
}

void AbstractModule::moduleRestoredHidden()
{
    widgetControlClicked(getModuleStatus()); //module was restored hidden and doesn't react corectly on first click.
}

void AbstractModule::setModuleStatus(ModuleStatus stat){
    ModuleStatus cur = moduleControlWidget->getStatus();
    bool isHidden = (cur == ModuleStatus::HIDDEN_PLAY ||
                     cur == ModuleStatus::HIDDEN_PAUSE ||
                     cur == ModuleStatus::HIDDEN_WAIT_EVENT);
    if (isHidden) {
        if (stat == ModuleStatus::PLAY)       stat = ModuleStatus::HIDDEN_PLAY;
        else if (stat == ModuleStatus::PAUSE) stat = ModuleStatus::HIDDEN_PAUSE;
        else if (stat == ModuleStatus::WAIT_EVENT) stat = ModuleStatus::HIDDEN_WAIT_EVENT;
    }
    moduleControlWidget->setStatus(stat);
}

ModuleStatus AbstractModule::getModuleStatus()
{
    return moduleControlWidget->getStatus();
}

void AbstractModule::setModuleConfigured(){
    isConfigured = true;
}

void AbstractModule::showModuleControl(){
    emit moduleControlShown(moduleName);
    moduleControlWidget->show();
}

void AbstractModule::showModuleDescription(QString name, QList<QString> labels, QList<QString> values)
{
    if(!moduleControlWidget->isHidden() && moduleDescriptionShown == false){
        emit moduleDescription(name, labels, values);
        moduleDescriptionShown = true;
    }
}

void AbstractModule::showModuleWindow(){
    dockWidgetWindow->show();
}

void AbstractModule::closeModule(){
    dockWidgetWindow->hide();

    if(moduleControlWidget->getStatus()!=ModuleStatus::STOP && moduleControlWidget->getStatus()!=ModuleStatus::LOCKED){
        stopModule();
        moduleControlWidget->setStatus(ModuleStatus::STOP);
        if(moduleSpecification != nullptr)
            emit releaseConflictingModules(moduleName, moduleSpecification->getResources());
    }
}

void AbstractModule::disableModule(){
    moduleControlWidget->hide();
    moduleRestored = false;
    moduleDescriptionShown = false;
    isConfigured = false;
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
    emit moduleNameChanged(moduleName);
}

/* This function must be called after dockWidget is created.
   Connect signal moduleCreated() in the module... */
void AbstractModule::showModuleHoldButton(bool show){
    if(dockWinCreated)
        dockWidgetWindow->showHoldButton(show);
}

void AbstractModule::enableModuleHoldButton(bool enable){
    dockWidgetWindow->enableHoldButton(enable);
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

