#include "panelmeasurement.h"

PanelMeasurement::PanelMeasurement(QVBoxLayout *destination, QWidget *parent)
{
    WidgetSeparator *sepa = new WidgetSeparator(parent,"Measurement");
    destination->addWidget(sepa);

    WidgetButtons *but2 = new WidgetButtons(parent,1,ButtonTypes::NORMAL,"Peak to peak");
    destination->addWidget(but2);
    but2->setText("  Add  ",0);

    connect(but2, &WidgetButtons::clicked, this, &PanelMeasurement::pkpkClicked);

    WidgetSeparator *separator = new WidgetSeparator(parent,"Separator - just some stuff to test");
    destination->addWidget(separator);

    channelButtons = new WidgetButtons(parent,4,ButtonTypes::RADIO,"Selection");
    destination->addWidget(channelButtons);
    channelButtons->setText("CH1",0);
    channelButtons->setColor(BUTTON_COLOR_ORANGE,0);
    channelButtons->setText("CH2",1);
    channelButtons->setColor(BUTTON_COLOR_BLUE,1);
    channelButtons->setText("CH3",2);
    channelButtons->setColor(BUTTON_COLOR_GREEN,2);
    channelButtons->setText("CH4",3);

    WidgetDialRange *valb = new WidgetDialRange(parent ,"Freq");
    valb->setRange(1000,10000000,"Hz",1,1000,10000,true);
    destination->addWidget(valb);

    WidgetDialRange *vala = new WidgetDialRange(parent ,"Voltage");
    vala->setRange(-3.3,3.3,"V",10,0.01);
    destination->addWidget(vala);

    WidgetSwitch *wsw = new WidgetSwitch(parent,"prvni");
    destination->addWidget(wsw);

    WidgetLabel *wlb = new WidgetLabel(parent, "Channel y enable");
    destination->addWidget(wlb);

    WidgetSelection *wsel = new WidgetSelection(parent,"Gain");
    wsel->addOption("1x",1);
    wsel->addOption("2x",2);
    wsel->addOption("4x",4);
    destination->addWidget(wsel);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}

void PanelMeasurement::pkpkClicked(){
    Measurement *m = new Measurement(MeasurementType::PKPK,channelButtons->getSelectedIndex()+1);
    emit measurementAdded(m);
}
