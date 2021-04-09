#include "syncpwmsettings.h"

SyncPwmSettings::SyncPwmSettings(QVBoxLayout *destination, SyncPwmConfig *config, QWidget *parent) :
    QObject(parent),
    config(config)
{        
    WidgetSeparator *separatorCommon = new WidgetSeparator(parent, "Common");
    destination->addWidget(separatorCommon);

    switchStepMode = new WidgetSwitch(parent, " Continuous ", "     Step     ", "");
    switchStepMode->setObjectName("syncPwmStepButton");
    switchStepMode->setColor(Graphics::getControlsColor());

    buttonStart = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0);
    buttonStart->setText("Start", 0);
    buttonStart->setChecked(false, 0);
    buttonStart->setObjectName("syncPwmStartButton");
    buttonStart->setColor(Graphics::getControlsColor(),0);

    buttonEquidist = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0);
    buttonEquidist->setText("Equidistant", 0);
    buttonEquidist->setObjectName("syncPwmEquidistButton");
    buttonEquidist->setChecked(false, 0);
    buttonEquidist->setColor(Graphics::getControlsColor(), 0);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout *commonButtons = new QHBoxLayout();
    destination->addLayout(commonButtons);
    commonButtons->addWidget(switchStepMode);
    commonButtons->addItem(spacer);
    commonButtons->addWidget(buttonStart);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    commonButtons->addItem(spacer);
    commonButtons->addWidget(buttonEquidist);

    /* Channels ------------------------------------------------------------- */
    int phase = 0;
    if(config->layout == Layout::HORIZONTAL){
        for(int i = 0; i < CHANNELS_NUM; i++){
            QString chNStr = QString::number(i+1);

            separator[i] = new WidgetSeparator(parent, "Channel " + chNStr);
            spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
            destination->addWidget(separator[i]);
            QHBoxLayout *horBoxButtons = new QHBoxLayout();
            QHBoxLayout *horBoxDials = new QHBoxLayout();
            destination->addLayout(horBoxButtons);
            destination->addLayout(horBoxDials);

            configControlElements(parent, i, phase);

            horBoxButtons->addWidget(onOffCh[i]);
            horBoxButtons->addItem(spacer);
            horBoxButtons->addWidget(inverCh[i]);
            horBoxDials->addWidget(dialFreqCh[i]);
            horBoxDials->addWidget(dialDutyCh[i]);
            horBoxDials->addWidget(dialPhaseCh[i]);

            phase += PI_HALF;
        }
    }else {
        QHBoxLayout *horBox = new QHBoxLayout();
        destination->addLayout(horBox);

        for(int i = 0; i < CHANNELS_NUM; i++){
            QString chNStr = QString::number(i+1);

            QVBoxLayout *verChanBox = new QVBoxLayout();
            separator[i] = new WidgetSeparator(parent, "Channel " + chNStr);

            configControlElements(parent, i, phase);

            horBox->addLayout(verChanBox);
            verChanBox->addWidget(separator[i]);
            verChanBox->addWidget(onOffCh[i]);
            verChanBox->addWidget(inverCh[i]);
            verChanBox->addWidget(dialFreqCh[i]);
            verChanBox->addWidget(dialDutyCh[i]);
            verChanBox->addWidget(dialPhaseCh[i]);

            phase += PI_HALF;
        }
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}

void SyncPwmSettings::configControlElements(QWidget *parent, int i, int phase){
    QString chNStr = QString::number(i+1);
    QString color = Graphics::getChannelColor(i);

    onOffCh[i] = new WidgetSwitch(parent, "On", "Off", "", i);
    onOffCh[i]->setObjectName("syncPwmOnOffCh" + chNStr);
    onOffCh[i]->setColor(color);

    inverCh[i] = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0, i);
    inverCh[i]->setText("Invert");
    inverCh[i]->setObjectName("syncPwmInverCh" + chNStr);
    inverCh[i]->setChecked(false, 0);
    inverCh[i]->setColor(color, 0);

    dialFreqCh[i] = new WidgetDialRange(parent, "Frequency", i);
    dialFreqCh[i]->setObjectName("syncPwmFreqCh" + chNStr);
    dialFreqCh[i]->setRange(0, 36000000, "Hz", 1, 1, DEFAULT_FREQUENCY, true);
    dialFreqCh[i]->setColor(color);
    dialFreqCh[i]->hideUnitSelection();

    dialDutyCh[i] = new WidgetDialRange(parent, "Duty cycle", i);
    dialDutyCh[i]->setObjectName("syncPwmDutyCh" + chNStr);
    dialDutyCh[i]->setRange(0, 100, "\%", 1, 1, DEFAULT_DC, false);
    dialDutyCh[i]->setColor(color);
    dialDutyCh[i]->hideUnitSelection();

    dialPhaseCh[i] = new WidgetDialRange(parent, "Phase", i);
    dialPhaseCh[i]->setObjectName("syncPwmPhaseCh" + chNStr);
    dialPhaseCh[i]->setRange(0, 360, "°", 1, 1, phase, false);
    dialPhaseCh[i]->setColor(color);
    dialPhaseCh[i]->hideUnitSelection();
}

void SyncPwmSettings::greyOutComplementChanFreqDials(int chanIndex){
    QString colDisable = Graphics::getButtonDisabledBackgroundColor();

    dialFreqCh[chanIndex]->setDisabled(true);
    dialFreqCh[chanIndex]->setColor(colDisable);
}


