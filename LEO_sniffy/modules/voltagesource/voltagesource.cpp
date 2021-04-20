#include "voltagesource.h"

VoltageSource::VoltageSource(QObject *parent)
{
    Q_UNUSED(parent);
    moduleSpecification = new VoltageSourceSpec();
    config = new VoltageSourceConfig();
    voltSourceWindow = new VoltageSourceWindow(config);
    voltSourceWindow->setObjectName("tmpWindow");

//Set the comm prefix, window name and icon
    //module is not fully initialized - control widget and dock wodget cannot be modified
    moduleCommandPrefix = cmd->VOLTAGE_SOURCE;
    moduleName = "Voltage source";
    moduleIconURI = Graphics::getGraphicsPath()+"icon_voltage_source.png";

    voltSourceWindow->setNumberOfChannels(2);
}

QWidget *VoltageSource::getWidget()
{
    return voltSourceWindow;
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
    voltSourceWindow->restoreGUIAfterStartup();
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

void VoltageSource::sendDACVoltage(int channelIndex, int dacValue)
{
    qDebug() << "voltage to be sent" <<dacValue <<channelIndex;
}






