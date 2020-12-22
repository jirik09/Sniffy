#include "templatemodule.h"

TemplateModule::TemplateModule(QObject *parent)
{
    Q_UNUSED(parent);
    specification = new TemplateModuleSpec();
    config = new TemplateModuleConfig();
    tempWindow = new TemplateModuleWindow(config);
    tempWindow->setObjectName("tmpWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "SYST";//cmd->SCOPE;
    moduleName = "Template Module";
    moduleIconURI = ":/graphics/graphics/icon_voltmeter.png";

//In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &TemplateModule::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &TemplateModule::holdButtonCallback);
}

QWidget *TemplateModule::getWidget()
{
    return tempWindow;
}

void TemplateModule::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        data.remove(0,4);
        specification->parseSpecification(data);
        showModuleControl();
//TODO parse message from MCU
    }else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void TemplateModule::writeConfiguration()
{
    tempWindow->restoreGUIAfterStartup();
//TODO this function is called when module is opened
}

void TemplateModule::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray TemplateModule::getConfiguration()
{
    return config->serialize();
}

void TemplateModule::startModule()
{
//TODO start the module
}

void TemplateModule::stopModule()
{
//TODO stop the module
}

//In case hold is needed

void TemplateModule::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void TemplateModule::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}

