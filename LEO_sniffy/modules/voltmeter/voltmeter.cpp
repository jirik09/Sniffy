#include "voltmeter.h"

Voltmeter::Voltmeter(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new VoltmeterSpec();
    config = new VoltmeterConfig();
    tempWindow = new VoltmeterWindow(config);
    tempWindow->setObjectName("tmpWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "SYST";//cmd->SCOPE;
    moduleName = "Template Module";
    moduleIconURI = ":/graphics/graphics/icon_voltmeter.png";

//In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &Voltmeter::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &Voltmeter::holdButtonCallback);
}

QWidget *Voltmeter::getWidget()
{
    return tempWindow;
}

void Voltmeter::parseData(QByteArray data)
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

void Voltmeter::writeConfiguration()
{
    tempWindow->restoreGUIAfterStartup();
//TODO this function is called when module is opened
}

void Voltmeter::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray Voltmeter::getConfiguration()
{
    return config->serialize();
}

void Voltmeter::startModule()
{
//TODO start the module
}

void Voltmeter::stopModule()
{
//TODO stop the module
}

//In case hold is needed

void Voltmeter::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void Voltmeter::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}

