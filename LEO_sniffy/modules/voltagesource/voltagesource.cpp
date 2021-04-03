#include "voltagesource.h"

VoltageSource::VoltageSource(QObject *parent, AbstractColors *theme)
{
    Q_UNUSED(parent);
    setTheme(theme);
    moduleSpecification = new VoltageSourceSpec();
    config = new VoltageSourceConfig();
    tempWindow = new VoltageSourceWindow(config);
    tempWindow->setObjectName("tmpWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "SYST";//cmd->SCOPE;
    moduleName = "Voltage source";
    moduleIconURI = ":/graphics/graphics/icon_voltage_source.png";

//In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &VoltageSource::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &VoltageSource::holdButtonCallback);
}

QWidget *VoltageSource::getWidget()
{
    return tempWindow;
}

void VoltageSource::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        data.remove(0,4);
        moduleSpecification->parseSpecification(data);
        showModuleControl();
//TODO parse message from MCU
    }else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void VoltageSource::writeConfiguration()
{
    tempWindow->restoreGUIAfterStartup();
//TODO this function is called when module is opened
}

void VoltageSource::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray VoltageSource::getConfiguration()
{
    return config->serialize();
}

void VoltageSource::startModule()
{
//TODO start the module
}

void VoltageSource::stopModule()
{
//TODO stop the module
}

//In case hold is needed

void VoltageSource::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void VoltageSource::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}

