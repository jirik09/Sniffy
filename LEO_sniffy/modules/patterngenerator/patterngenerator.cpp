#include "patterngenerator.h"

PatternGenerator::PatternGenerator(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new PatternGeneratorSpec();
    config = new PatternGeneratorConfig();
    tempWindow = new PatternGeneratorWindow(config);
    tempWindow->setObjectName("tmpWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "SYST";//cmd->SCOPE;
    moduleName = "Pattern generator";
    moduleIconURI = ":/graphics/graphics/icon_pattern_generator.png";

//In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &PatternGenerator::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &PatternGenerator::holdButtonCallback);
}

QWidget *PatternGenerator::getWidget()
{
    return tempWindow;
}

void PatternGenerator::parseData(QByteArray data)
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

void PatternGenerator::writeConfiguration()
{
    tempWindow->restoreGUIAfterStartup();
//TODO this function is called when module is opened
}

void PatternGenerator::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray PatternGenerator::getConfiguration()
{
    return config->serialize();
}

void PatternGenerator::startModule()
{
//TODO start the module
}

void PatternGenerator::stopModule()
{
//TODO stop the module
}

//In case hold is needed

void PatternGenerator::showHoldButtonCallback(){
    this->showModuleHoldButton();
}

void PatternGenerator::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}

