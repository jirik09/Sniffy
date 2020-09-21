#include "countertablowfreq.h"

CounterTabLowFreq::CounterTabLowFreq(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{        
    WidgetSeparator *separatorChannelSwitch = new WidgetSeparator(parent, "Channel configuration");
    destination->addWidget(separatorChannelSwitch);

    buttonsChannelSwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsChannelSwitch->setText("CH1", 0);
    buttonsChannelSwitch->setText("CH2", 1);
    buttonsChannelSwitch->setColor(lfCh2BckgndColor, 1);
    destination->addWidget(buttonsChannelSwitch);

    WidgetSeparator *separatorQuantitySwitch = new WidgetSeparator(parent,"Quantity switch");
    destination->addWidget(separatorQuantitySwitch);

    buttonsQuantitySwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsQuantitySwitch->setText("Frequency", 0);
    buttonsQuantitySwitch->setText("Period", 1);
    destination->addWidget(buttonsQuantitySwitch);

    dialSampleCountCh1 = new WidgetDialRange(parent ,"Sample Count");
    dialSampleCountCh1->setRange(1, SAMPLE_COUNT, "Smpls", 1, 1, 1, false);
    dialSampleCountCh1->hideUnitSelection();
    destination->addWidget(dialSampleCountCh1);

    dialSampleCountCh2 = new WidgetDialRange(parent ,"Sample Count");
    dialSampleCountCh2->setRange(1, SAMPLE_COUNT, "Smpls", 1, 1, 1, false);
    dialSampleCountCh2->hideUnitSelection();
    destination->addWidget(dialSampleCountCh2);
    QString colorStyleSheet = lfCh2BckgndColor;
    dialSampleCountCh2->setDialButtonsColor(colorStyleSheet);
    colorStyleSheet = lfCh2TextColor;
    dialSampleCountCh2->setDialColor(colorStyleSheet);
    dialSampleCountCh2->hide();

    WidgetSeparator *separatorMultiplierSwitch = new WidgetSeparator(parent,"Sample Count multiplier");
    destination->addWidget(separatorMultiplierSwitch);

    buttonsMultiplierSwitch = new WidgetButtons(parent, 4, ButtonTypes::RADIO, "", 0);
    buttonsMultiplierSwitch->setText("1x", 0);
    buttonsMultiplierSwitch->setText("2x", 1);
    buttonsMultiplierSwitch->setText("4x", 2);
    buttonsMultiplierSwitch->setText("8x", 3);
    destination->addWidget(buttonsMultiplierSwitch);

    WidgetSeparator *separatorDutyCycleSwitch = new WidgetSeparator(parent,"Switch to Duty cycle");
    destination->addWidget(separatorDutyCycleSwitch);

    buttonsDutyCycleSwitch = new WidgetButtons(parent, 2, ButtonTypes::RADIO, "", 0);
    buttonsDutyCycleSwitch->setText("Disable", 0);
    buttonsDutyCycleSwitch->setText("Enable", 1);
    destination->addWidget(buttonsDutyCycleSwitch);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}

void CounterTabLowFreq::enableAllComponents(bool enable)
{
    buttonsChannelSwitch->enableAll(enable);
    buttonsQuantitySwitch->enableAll(enable);
    buttonsMultiplierSwitch->enableAll(enable);
    if(buttonsChannelSwitch->isChecked(CHANNEL_1)){
        dialSampleCountCh1->setEnabled(enable);
    }else {
        dialSampleCountCh2->setEnabled(enable);
    }
}

void CounterTabLowFreq::changeAllComponentsColor(QString bckgndColor)
{
    buttonsQuantitySwitch->setColor(bckgndColor, 0);
    buttonsQuantitySwitch->setColor(bckgndColor, 1);
    buttonsMultiplierSwitch->setColor(bckgndColor, 0);
    buttonsMultiplierSwitch->setColor(bckgndColor, 1);
    buttonsMultiplierSwitch->setColor(bckgndColor, 2);
    buttonsMultiplierSwitch->setColor(bckgndColor, 3);
    buttonsDutyCycleSwitch->setColor(bckgndColor, 0);
    buttonsDutyCycleSwitch->setColor(bckgndColor, 1);
}

void CounterTabLowFreq::changeAllComponentsCheck(int quantBtnIndex, int dcBtnIndex, int multipIndex)
{
    buttonsQuantitySwitch->setChecked(true, quantBtnIndex);
    buttonsDutyCycleSwitch->setChecked(true, dcBtnIndex);
    buttonsMultiplierSwitch->setChecked(true, multipIndex);
}

void CounterTabLowFreq::showDialInChannel(int channel, bool visible)
{
    if(channel == 0){
        (visible) ? dialSampleCountCh1->show() : dialSampleCountCh1->hide();
    }else if (channel == 1) {
        (visible) ? dialSampleCountCh2->show() : dialSampleCountCh2->hide();
    }
}

