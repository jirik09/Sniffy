#include "panelmeasurement.h"

PanelMeasurement::PanelMeasurement(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Measurement"));

    channelButtons = new WidgetButtons(parent,4,ButtonTypes::RADIO,"Channel selection");
    destination->addWidget(channelButtons);
    channelButtons->setText("CH1",0);
    channelButtons->setColor(BUTTON_COLOR_ORANGE,0);
    channelButtons->setText("CH2",1);
    channelButtons->setColor(BUTTON_COLOR_BLUE,1);
    channelButtons->setText("CH3",2);
    channelButtons->setColor(BUTTON_COLOR_GREEN,2);
    channelButtons->setText("CH4",3);
    channelButtons->setColor(BUTTON_COLOR_PURPLE,3);

    destination->addWidget(new WidgetSeparator(parent,"Voltage measurement"));

    WidgetButtons *butRMS = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"RMS");
    destination->addWidget(butRMS);
    measButtons.append(butRMS);
    butRMS->setText("  Add  ",0);

    WidgetButtons *butMean = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Mean");
    destination->addWidget(butMean);
    measButtons.append(butMean);
    butMean->setText("  Add  ",0);

    WidgetButtons *butPkPk = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Pk Pk");
    destination->addWidget(butPkPk);
    measButtons.append(butPkPk);
    butPkPk->setText("  Add  ",0);

    WidgetButtons *butMax = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Max");
    destination->addWidget(butMax);
    measButtons.append(butMax);
    butMax->setText("  Add  ",0);

    WidgetButtons *butMin = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Min");
    destination->addWidget(butMin);
    measButtons.append(butMin);
    butMin->setText("  Add  ",0);


    destination->addWidget(new WidgetSeparator(parent,"Time measurement"));

    WidgetButtons *butFreq = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Frequency");
    destination->addWidget(butFreq);
    measButtons.append(butFreq);
    butFreq->setText("  Add  ",0);

    WidgetButtons *butPeriod = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Period");
    destination->addWidget(butPeriod);
    measButtons.append(butPeriod);
    butPeriod->setText("  Add  ",0);

    WidgetButtons *butDuty = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Duty");
    destination->addWidget(butDuty);
    measButtons.append(butDuty);
    butDuty->setText("  Add  ",0);

    WidgetButtons *butHigh = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"High");
    destination->addWidget(butHigh);
    measButtons.append(butHigh);
    butHigh->setText("  Add  ",0);

    WidgetButtons *butLow = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Low");
    destination->addWidget(butLow);
    measButtons.append(butLow);
    butLow->setText("  Add  ",0);

    destination->addWidget(new WidgetSeparator(parent,"Phase measurement"));

    channelButtonPhaseA = new WidgetButtons(parent,4,ButtonTypes::RADIO,"1st channel");
    destination->addWidget(channelButtonPhaseA);
    channelButtonPhaseA->setText("CH1",0);
    channelButtonPhaseA->setColor(BUTTON_COLOR_ORANGE,0);
    channelButtonPhaseA->setText("CH2",1);
    channelButtonPhaseA->setColor(BUTTON_COLOR_BLUE,1);
    channelButtonPhaseA->setText("CH3",2);
    channelButtonPhaseA->setColor(BUTTON_COLOR_GREEN,2);
    channelButtonPhaseA->setText("CH4",3);
    channelButtonPhaseA->setColor(BUTTON_COLOR_PURPLE,3);

    channelButtonPhaseB = new WidgetButtons(parent,4,ButtonTypes::RADIO,"2nd channel");
    destination->addWidget(channelButtonPhaseB);
    channelButtonPhaseB->setText("CH1",0);
    channelButtonPhaseB->setColor(BUTTON_COLOR_ORANGE,0);
    channelButtonPhaseB->setText("CH2",1);
    channelButtonPhaseB->setColor(BUTTON_COLOR_BLUE,1);
    channelButtonPhaseB->setText("CH3",2);
    channelButtonPhaseB->setColor(BUTTON_COLOR_GREEN,2);
    channelButtonPhaseB->setText("CH4",3);
    channelButtonPhaseB->setColor(BUTTON_COLOR_PURPLE,3);
    channelButtonPhaseB->setChecked(true,1);



    WidgetButtons *butPhase = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Phase");
    destination->addWidget(butPhase);
    butPhase->setText("  Add  ",0);

    destination->addWidget(new WidgetSeparator(parent,""));

    WidgetButtons *butClear = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"");
    destination->addWidget(butClear);
    butClear->setText("  Clear all measuremts  ",0);

    setMeasButtonsColor(0);

    connect(channelButtons, &WidgetButtons::clicked, this, &PanelMeasurement::setMeasButtonsColor);

    connect(butRMS, &WidgetButtons::clicked, this, &PanelMeasurement::RMSClicked);
    connect(butMean, &WidgetButtons::clicked, this, &PanelMeasurement::meanClicked);
    connect(butMax, &WidgetButtons::clicked, this, &PanelMeasurement::maxClicked);
    connect(butMin, &WidgetButtons::clicked, this, &PanelMeasurement::minClicked);
    connect(butPkPk, &WidgetButtons::clicked, this, &PanelMeasurement::pkpkClicked);

    connect(butFreq, &WidgetButtons::clicked, this, &PanelMeasurement::freqClicked);
    connect(butPeriod, &WidgetButtons::clicked, this, &PanelMeasurement::periodClicked);
    connect(butPhase, &WidgetButtons::clicked, this, &PanelMeasurement::phaseClicked);
    connect(butDuty, &WidgetButtons::clicked, this, &PanelMeasurement::dutyClicked);
    connect(butHigh, &WidgetButtons::clicked, this, &PanelMeasurement::highClicked);
    connect(butLow, &WidgetButtons::clicked, this, &PanelMeasurement::lowClicked);

    connect(butClear, &WidgetButtons::clicked, this, &PanelMeasurement::clearClicked);


}

void PanelMeasurement::RMSClicked(){
    Measurement *m = new Measurement(MeasurementType::RMS,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::meanClicked(){
    Measurement *m = new Measurement(MeasurementType::MEAN,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::maxClicked(){
    Measurement *m = new Measurement(MeasurementType::MAX,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::minClicked(){
    Measurement *m = new Measurement(MeasurementType::MIN,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::pkpkClicked(){
    Measurement *m = new Measurement(MeasurementType::PKPK,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::freqClicked(){
    Measurement *m = new Measurement(MeasurementType::FREQUENCY,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::periodClicked(){
    Measurement *m = new Measurement(MeasurementType::PERIOD,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::dutyClicked(){
    Measurement *m = new Measurement(MeasurementType::DUTY,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::highClicked(){
    Measurement *m = new Measurement(MeasurementType::HIGH,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::lowClicked(){
    Measurement *m = new Measurement(MeasurementType::LOW,channelButtons->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::phaseClicked(){
    Measurement *m = new Measurement(MeasurementType::PHASE,channelButtonPhaseA->getSelectedIndex()*10 + channelButtonPhaseB->getSelectedIndex());
    emit measurementAdded(m);
}

void PanelMeasurement::clearClicked(){
    emit measurementClearAll();
}

void PanelMeasurement::setMeasButtonsColor(int index){
    foreach(WidgetButtons* but, measButtons){
        if(index==0)
            but->setColor(QString::fromUtf8(BUTTON_COLOR_ORANGE),0);
        if(index==1)
            but->setColor(QString::fromUtf8(BUTTON_COLOR_BLUE),0);
        if(index==2)
            but->setColor(QString::fromUtf8(BUTTON_COLOR_GREEN),0);
        if(index==3)
            but->setColor(QString::fromUtf8(BUTTON_COLOR_PURPLE),0);
    }

}
