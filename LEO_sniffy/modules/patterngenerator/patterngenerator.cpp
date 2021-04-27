#include "patterngenerator.h"

PatternGenerator::PatternGenerator(QObject *parent)
{
    Q_UNUSED(parent);

    config = new PatternGeneratorConfig();
    pattGenWindow = new PatternGeneratorWindow(config);
    pattGenWindow->setObjectName("patternGenWindow");

    moduleCommandPrefix = cmd->PATTERN_GENERATOR;
    moduleName = "Pattern generator";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_pattern_generator.png";

    connect(this, &AbstractModule::moduleCreated, this, &PatternGenerator::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &PatternGenerator::holdButtonCallback);
}

QWidget *PatternGenerator::getWidget()
{
    return pattGenWindow;
}

void PatternGenerator::parseData(QByteArray data){
    QByteArray dataHeader = data.left(4);
    QByteArray dataToPass = data.remove(0, 4);

    if(dataHeader==cmd->CONFIG){
        moduleSpecification = new PatternGeneratorSpec(this);
        moduleSpecification->parseSpecification(dataToPass);
        pattGenWindow->setSpecification(static_cast<PatternGeneratorSpec*>(moduleSpecification));
        showModuleControl();

    }else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void PatternGenerator::writeConfiguration()
{
    pattGenWindow->restoreGUIAfterStartup();
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

}

void PatternGenerator::stopModule()
{

}

void PatternGenerator::showHoldButtonCallback(){
    this->showModuleHoldButton(true);
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

