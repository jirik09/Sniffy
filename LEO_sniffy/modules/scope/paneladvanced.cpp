#include "paneladvanced.h"

PanelAdvanced::PanelAdvanced(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Resolution"));

    resolutionButtons = new WidgetButtons(parent,2,ButtonTypes::RADIO,"");
    resolutionButtons->setObjectName("scoperesolution");
    destination->addWidget(resolutionButtons);
    resolutionButtons->setText("8 bit",0);
    resolutionButtons->setText("12 bit",1);

    destination->addWidget(new WidgetSeparator(parent,"Sampling"));

    samplingOverwriteButtons = new WidgetButtons(parent,2,ButtonTypes::RADIO,"");
    samplingOverwriteButtons->setObjectName("scoperesolution");
    destination->addWidget(samplingOverwriteButtons);
    samplingOverwriteButtons->setText("Auto",0);
    samplingOverwriteButtons->setText("Overwrite",1);

    samlingFrequencyInput = new WidgetTextInput(parent,"Sampling","10000");
    destination->addWidget(samlingFrequencyInput);

    dataLengthInput = new WidgetTextInput(parent,"Length","10000");
    destination->addWidget(dataLengthInput);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

}
