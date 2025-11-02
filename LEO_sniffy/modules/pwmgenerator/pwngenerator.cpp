#include "pwmgenerator.h"

PWMGenerator::PWMGenerator(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new PWMGeneratorSpec();
    config = new PWMGeneratorConfig();
    tempWindow = new PWMGeneratorWindow(config);
    tempWindow->setObjectName("tmpWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "SYST";//cmd->SCOPE;
    moduleName = "PWM genarator";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_pwm_gen.png";

//In case hold button should be shown insert this and connect callback to handle hold/pause
    connect(this, &AbstractModule::moduleCreated, this, &PWMGenerator::showHoldButtonCallback);
    connect(this, &AbstractModule::holdClicked, this, &PWMGenerator::holdButtonCallback);
}

QWidget *PWMGenerator::getWidget()
{
    return tempWindow;
}

void PWMGenerator::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader==Commands::CONFIG){
        data.remove(0,4);
        moduleSpecification->parseSpecification(data);
        showModuleControl();
//TODO parse message from MCU
    }else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void PWMGenerator::writeConfiguration()
{
    tempWindow->restoreGUIAfterStartup();
//TODO this function is called when module is opened
}

void PWMGenerator::parseConfiguration(QByteArray config)
{
    this->config->parse(config);
}

QByteArray PWMGenerator::getConfiguration()
{
    return config->serialize();
}

void PWMGenerator::startModule()
{
//TODO start the module
}

void PWMGenerator::stopModule()
{
//TODO stop the module
}

//In case hold is needed

void PWMGenerator::showHoldButtonCallback(){
    this->showModuleHoldButton(true);
}

void PWMGenerator::holdButtonCallback(bool held){
    if(held){
        comm->write(moduleCommandPrefix+":"+cmd->PAUSE+";");
        setModuleStatus(ModuleStatus::PAUSE);
    }else{
        comm->write(moduleCommandPrefix+":"+cmd->UNPAUSE+";");
        setModuleStatus(ModuleStatus::PLAY);
    }
}

