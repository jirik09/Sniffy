#include "arbgenerator.h"

ArbGenerator::ArbGenerator(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new ArbGeneratorSpec();
    config = new ArbGeneratorConfig();
    arbGenWindow = new ArbGeneratorWindow(config);
    arbGenWindow->setObjectName("arbGenWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = "GEN_";//cmd->SCOPE;
    moduleName = "Arbitrary generator";
    moduleIconURI = ":/graphics/graphics/icon_signal_generator.png";
}

QWidget *ArbGenerator::getWidget()
{
    return arbGenWindow;
}

void ArbGenerator::parseData(QByteArray data)
{
    QByteArray dataHeader = data.left(4);

    if(dataHeader=="CFG_"){
        data.remove(0,4);
        moduleSpecification->parseSpecification(data);
        arbGenWindow->setSpecification(static_cast<ArbGeneratorSpec*>(moduleSpecification));
        showModuleControl();
//TODO parse message from MCU
    }else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void ArbGenerator::writeConfiguration()
{
    arbGenWindow->restoreGUIAfterStartup();
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


