#include "arbgenerator.h"

ArbGenerator::ArbGenerator(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new ArbGeneratorSpec();
    config = new ArbGeneratorConfig();
    tempWindow = new ArbGeneratorWindow(config);
    tempWindow->setObjectName("arbGenWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "SYST";//cmd->SCOPE;
    moduleName = "Arbitrary generator";
    moduleIconURI = ":/graphics/graphics/icon_signal_generator.png";

//In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &ArbGenerator::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &ArbGenerator::holdButtonCallback);
}

QWidget *ArbGenerator::getWidget()
{
    return tempWindow;
}

void ArbGenerator::parseData(QByteArray data)
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

void ArbGenerator::writeConfiguration()
{
    tempWindow->restoreGUIAfterStartup();
//TODO this function is called when module is opened
}

void ArbGenerator::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray ArbGenerator::getConfiguration()
{
    return config->serialize();
}

void ArbGenerator::startModule()
{
//TODO start the module
}

void ArbGenerator::stopModule()
{
//TODO stop the module
}

//In case hold is needed

void ArbGenerator::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void ArbGenerator::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}

