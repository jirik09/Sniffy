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

    //connect(pattGenWindow->settings->buttonStart, &WidgetButtons::clicked, this, &PatternGenerator::startGenerator());
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

    }else if(dataHeader==cmd->CMD_GEN_NEXT){
        dataTransferNext();

    }else if(dataHeader==cmd->CMD_GEN_OK){
        dataTransferFinished();

    }else{
        qDebug()<<"[PATTERN GEN] Unhandled incomming data!"<<data;
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
    setModuleStatus(ModuleStatus::PAUSE);
}

void PatternGenerator::stopModule()
{
    stopGenerator();
    genComms->generatorDeinit();
    pattGenWindow->setGeneratorState(false);
}

void PatternGenerator::startGenerator()
{
    setModuleStatus(ModuleStatus::PLAY);
    genComms->startGenerator();
}

void PatternGenerator::stopGenerator()
{
    setModuleStatus(ModuleStatus::PAUSE);
    genComms->stopGenerator();
}

void PatternGenerator::dataTransferNext()
{
    if(!dataBeingUploaded)
        return;

    genComms->sendNext();
    if(genComms->isSentAll()){
        genComms->genAskForFreq();
        startGenerator();
    }

    pattGenWindow->setProgress(genComms->getProgress());
}

void PatternGenerator::dataTransferFinished()
{
    if(dataBeingUploaded){
        dataBeingUploaded = false;
        pattGenWindow->setGeneratorState(true);
    }
}


