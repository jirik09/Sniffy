#include "syncpwmsettings.h"

SyncPwmSettings::SyncPwmSettings(QVBoxLayout *destination, SyncPwmConfig *config, QWidget *parent) :
    QObject(parent),
    config(config)
{
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    WidgetSeparator *separatorCommon = new WidgetSeparator(parent, "Common");
    destination->addWidget(separatorCommon);

    switchStepMode = new WidgetSwitch(parent, " Continuous ", "    Step    ", "");
    switchStepMode->setObjectName("syncPwmStepButton");

    buttonStart = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0);
    buttonStart->setText("Start", 0);
    buttonStart->setChecked(false, 0);
    buttonStart->setObjectName("syncPwmStartButton");
    //QString bckgndColor = "background-color:"+QString::fromUtf8(COLOR_DARK_CYAN);
    //buttonStart->setColor(bckgndColor, 0);

    buttonEquidist = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0);
    buttonEquidist->setText("Equidistant", 0);
    buttonEquidist->setObjectName("syncPwmEquidistButton");
    buttonEquidist->setChecked(false, 0);

    QHBoxLayout *commonButtons = new QHBoxLayout();
    destination->addLayout(commonButtons);
    commonButtons->addWidget(switchStepMode);
    commonButtons->addItem(spacer);
    commonButtons->addWidget(buttonStart);
    commonButtons->addItem(spacer);
    commonButtons->addWidget(buttonEquidist);

    /* Channels ------------------------------------------------------------- */
    int phase = 0;

    if(config->layout == Layout::HORIZONTAL){

        for(int i = 0; i < CHANNELS_NUM; i++){
            QString chNStr = QString::number(i+1);

            WidgetSeparator *separator = new WidgetSeparator(parent, "Channel " + chNStr);
            destination->addWidget(separator);

            QHBoxLayout *horBoxButtons = new QHBoxLayout();
            QHBoxLayout *horBoxDials = new QHBoxLayout();
            destination->addLayout(horBoxButtons);
            destination->addLayout(horBoxDials);

            onOffCh[i] = new WidgetSwitch(parent, "On", "Off", "", i);
            onOffCh[i]->setObjectName("syncPwmOnOffCh" + chNStr);
            onOffCh[i]->setColor("background-color:" + chanColor[i]);
            horBoxButtons->addWidget(onOffCh[i]);

            horBoxButtons->addItem(spacer);

            inverCh[i] = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0, i);
            inverCh[i]->setText("Invert");
            inverCh[i]->setObjectName("syncPwmInverCh" + chNStr);
            inverCh[i]->setChecked(false, 0);
            inverCh[i]->setColor("background-color:" + chanColor[i], 0);
            horBoxButtons->addWidget(inverCh[i]);

            dialFreqCh[i] = new WidgetDialRange(parent, "Frequency", i);
            dialFreqCh[i]->setObjectName("syncPwmFreqCh" + chNStr);
            dialFreqCh[i]->setRange(0, 36000000, "Hz", 1, 1, DEFAULT_FREQUENCY, false);
            dialFreqCh[i]->setColor(chanColor[i]);
            dialFreqCh[i]->hideUnitSelection();
            horBoxDials->addWidget(dialFreqCh[i]);

            dialDutyCh[i] = new WidgetDialRange(parent, "Duty cycle", i);
            dialDutyCh[i]->setObjectName("syncPwmDutyCh" + chNStr);
            dialDutyCh[i]->setRange(0, 100, "\%", 1, 1, DEFAULT_DC, false);
            dialDutyCh[i]->setColor(chanColor[i]);
            dialDutyCh[i]->hideUnitSelection();
            horBoxDials->addWidget(dialDutyCh[i]);

            dialPhaseCh[i] = new WidgetDialRange(parent, "Phase", i);
            dialPhaseCh[i]->setObjectName("syncPwmPhaseCh" + chNStr);
            dialPhaseCh[i]->setRange(0, 360, "°", 1, 1, phase, false);
            dialPhaseCh[i]->setColor(chanColor[i]);
            dialPhaseCh[i]->hideUnitSelection();
            horBoxDials->addWidget(dialPhaseCh[i]);

            phase += PI_HALF;
        }

    }else {

        QHBoxLayout *horBox = new QHBoxLayout();
        destination->addLayout(horBox);

        for(int i = 0; i < CHANNELS_NUM; i++){
            QString chNStr = QString::number(i+1);

            QVBoxLayout *verChanBox = new QVBoxLayout();
            horBox->addLayout(verChanBox);

            WidgetSeparator *separator = new WidgetSeparator(parent, "Channel " + chNStr);
            verChanBox->addWidget(separator);

            onOffCh[i] = new WidgetSwitch(parent, "On", "Off", "", i);
            onOffCh[i]->setObjectName("syncPwmOnOffCh" + chNStr);
            onOffCh[i]->setColor("background-color:" + chanColor[i]);
            verChanBox->addWidget(onOffCh[i]);

            inverCh[i] = new WidgetButtons(parent, 1, ButtonTypes::CHECKABLE, "", 0, i);
            inverCh[i]->setText("Invert");
            inverCh[i]->setObjectName("syncPwmInverCh" + chNStr);
            inverCh[i]->setChecked(false, 0);
            inverCh[i]->setColor("background-color:" + chanColor[i], 0);
            verChanBox->addWidget(inverCh[i]);

            dialFreqCh[i] = new WidgetDialRange(parent, "Frequency", i);
            dialFreqCh[i]->setObjectName("syncPwmFreqCh" + chNStr);
            dialFreqCh[i]->setRange(0, 36000000, "Hz", 1, 1, DEFAULT_FREQUENCY, false);
            dialFreqCh[i]->setColor(chanColor[i]);
            dialFreqCh[i]->hideUnitSelection();
            verChanBox->addWidget(dialFreqCh[i]);

            dialDutyCh[i] = new WidgetDialRange(parent, "Duty cycle", i);
            dialDutyCh[i]->setObjectName("syncPwmDutyCh" + chNStr);
            dialDutyCh[i]->setRange(0, 100, "\%", 1, 1, DEFAULT_DC, false);
            dialDutyCh[i]->setColor(chanColor[i]);
            dialDutyCh[i]->hideUnitSelection();
            verChanBox->addWidget(dialDutyCh[i]);

            dialPhaseCh[i] = new WidgetDialRange(parent, "Phase", i);
            dialPhaseCh[i]->setObjectName("syncPwmPhaseCh" + chNStr);
            dialPhaseCh[i]->setRange(0, 360, "°", 1, 1, phase, false);
            dialPhaseCh[i]->setColor(chanColor[i]);
            dialPhaseCh[i]->hideUnitSelection();
            verChanBox->addWidget(dialPhaseCh[i]);

            phase += PI_HALF;
        }
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}

void SyncPwmSettings::greyOutComplementChanFreqDials(int chanIndex){
    QString colDisable = BACKGROUND_COLOR_BUTTON_DISABLED;
    dialFreqCh[chanIndex]->setDisabled(true);
    dialFreqCh[chanIndex]->setColor(colDisable);
}
