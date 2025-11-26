#include "paneladvanced.h"
#include "scopedefs.h"

PanelAdvanced::PanelAdvanced(QVBoxLayout *destination, QWidget *parent)
{
    destination->addWidget(new WidgetSeparator(parent,"Resolution"));

    resolutionButtons = new WidgetButtons(parent,2,ButtonTypes::RADIO,"",1);
    resolutionButtons->setObjectName("scoperesolution");
    destination->addWidget(resolutionButtons);
    resolutionButtons->setText("8 bit",0);
    resolutionButtons->setText("12 bit",1);

    destination->addWidget(new WidgetSeparator(parent,"Mode"));
    modeButtons = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    modeButtons->setText("Normal", 0);
    modeButtons->setText("XY", 1);
    destination->addWidget(modeButtons);

    channelXButtons = new WidgetButtons(parent, MAX_SCOPE_CHANNELS, ButtonTypes::RADIO, "Ch X", 0);
    for(int i=0; i<MAX_SCOPE_CHANNELS; i++) {
        channelXButtons->setText(QString("CH%1").arg(i+1), i);
        channelXButtons->setColor(Graphics::getChannelColor(i), i);
    }
    destination->addWidget(channelXButtons);

    channelYButtons = new WidgetButtons(parent, MAX_SCOPE_CHANNELS, ButtonTypes::RADIO, "Ch Y", 1);
    for(int i=0; i<MAX_SCOPE_CHANNELS; i++) {
        channelYButtons->setText(QString("CH%1").arg(i+1), i);
        channelYButtons->setColor(Graphics::getChannelColor(i), i);
    }
    destination->addWidget(channelYButtons);

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
