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
    connect(voltSourceWindow,&VoltageSourceWindow::voltageChanged,this,&VoltageSource::voltageChangedCallback);
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
        spec = static_cast<VoltageSourceSpec*>(moduleSpecification);
        showModuleControl();
        //TODO parse message from MCU
    }else if(dataHeader=="xxxx"){

    }else{
        //qDebug()<<data;
    }
}

void VoltageSource::writeConfiguration()
{
    voltSourceStart();
    voltSourceWindow->restoreGUIAfterStartup();
    voltSourceWindow->setPins(static_cast<VoltageSourceSpec*>(moduleSpecification)->channelPins,MAX_VOLTAGE_SOURCE_CHANNELS);
    voltSourceWindow->setRange(spec->rangeMin,spec->rangeMax);
    voltSourceWindow->setRealVdda(spec->AVddReal);

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
}

void VoltageSource::stopModule()
{
    voltSourceStop();
}

void VoltageSource::voltageChangedCallback(qreal value, int channel)
{
    int tmpDAC = ((pow(2,spec->DACResolution))*(qreal)(value-spec->rangeMin))/(spec->rangeMax-spec->rangeMin) * spec->VddDefault/spec->AVddReal;
    if (tmpDAC>pow(2,spec->DACResolution)-1) tmpDAC = pow(2,spec->DACResolution)-1;

    DACData[channel] = tmpDAC;

    qreal tmpValue = (qreal(tmpDAC)/(pow(2,spec->DACResolution)-1)*(spec->rangeMax-spec->rangeMin)+spec->rangeMin) * spec->AVddReal/spec->VddDefault;
    // Update numeric display with reconstructed actual value
    voltSourceWindow->setDisplayValue(tmpValue,channel);
    // Provide raw voltage (value) to bar; bar will map using dial range only once.
    voltSourceWindow->setBarValue(value,channel);

    sendDACVoltage();

}

void VoltageSource::sendDACVoltage()
{
    comm->write(cmd->VOLTAGE_SOURCE+":"+cmd->CMD_GEN_DAC_VAL+ " ");
    QByteArray tmpHeader;
    QDataStream dataStreamHeader(&tmpHeader, QIODevice::WriteOnly);

    for(int i = 0;i<spec->maxDACChannels;i++){
        qint16  sample = qFromBigEndian<qint16>(DACData[i]);
        dataStreamHeader << sample;
    }
    comm->write(tmpHeader+";");
}

void VoltageSource::voltSourceStop()
{
    comm->write(cmd->VOLTAGE_SOURCE+":"+cmd->STOP+ ";");
}

void VoltageSource::voltSourceStart()
{
    comm->write(cmd->VOLTAGE_SOURCE+":"+cmd->START+ ";");
}









