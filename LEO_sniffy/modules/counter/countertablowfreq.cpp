#include "countertablowfreq.h"

CounterTabLowFreq::CounterTabLowFreq(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{    
    WidgetSeparator *separatorChannelSwitch = new WidgetSeparator(parent, "Channel configuration");
    destination->addWidget(separatorChannelSwitch);

    buttonsChannelSwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsChannelSwitch->setText("CH1", 0);
    buttonsChannelSwitch->setText("CH2", 1);
    buttonsChannelSwitch->setColor(BCKGRND_COLOR_GREEN, 1);
    destination->addWidget(buttonsChannelSwitch);

    WidgetSeparator *separatorQuantitySwitch = new WidgetSeparator(parent,"Quantity switch");
    destination->addWidget(separatorQuantitySwitch);

    buttonsQuantitySwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsQuantitySwitch->setText("Frequency", 0);
    buttonsQuantitySwitch->setText("Period", 1);
    destination->addWidget(buttonsQuantitySwitch);

    dialSampleCount = new WidgetDialRange(parent ,"Sample Count");
    dialSampleCount->setRange(1, SAMPLE_COUNT, "Samples", 1, 100, 2, false);
    dialSampleCount->hideUnitSelection();
    destination->addWidget(dialSampleCount);

    WidgetSeparator *separatorMultiplierSwitch = new WidgetSeparator(parent,"Quantity multiplier");
    destination->addWidget(separatorMultiplierSwitch);

    buttonsSampleCountMultiplierSwitch = new WidgetButtons(parent, 4, ButtonTypes::RADIO, "", 0);
    buttonsSampleCountMultiplierSwitch->setText("1x", 0);
    buttonsSampleCountMultiplierSwitch->setText("2x", 1);
    buttonsSampleCountMultiplierSwitch->setText("4x", 2);
    buttonsSampleCountMultiplierSwitch->setText("8x", 3);
    destination->addWidget(buttonsSampleCountMultiplierSwitch);

    WidgetSeparator *separatorDutyCycleSwitch = new WidgetSeparator(parent,"Switch to Duty cycle");
    destination->addWidget(separatorDutyCycleSwitch);

    buttonsDutyCycleSwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsDutyCycleSwitch->setText("Show", 0);
    buttonsDutyCycleSwitch->setText("Hide", 1);
    destination->addWidget(buttonsDutyCycleSwitch);
}

