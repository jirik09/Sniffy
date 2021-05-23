#include "paneladvanced.h"

PanelAdvanced::PanelAdvanced(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Resolution"));

    resolutionButtons = new WidgetButtons(parent,2,ButtonTypes::RADIO,"",1);
    resolutionButtons->setObjectName("scoperesolution");
    destination->addWidget(resolutionButtons);
    resolutionButtons->setText("8 bit",0);
    resolutionButtons->setText("12 bit",1);

    destination->addWidget(new WidgetSeparator(parent,"Sampling"));

    samplingFrequencyInput = new WidgetTextInput(parent,"Sampling","100k",InputTextType::NUMBER);
    destination->addWidget(samplingFrequencyInput);

    samplingFrequencyReal = new WidgetLabel(parent,"Real Sampling","--");
    destination->addWidget(samplingFrequencyReal);

    dataLengthInput = new WidgetTextInput(parent,"Length","1000",InputTextType::NUMBER);
    destination->addWidget(dataLengthInput);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);

}
