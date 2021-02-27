#include "counterwindow.h"
#include "ui_counterwindow.h"

CounterWindow::CounterWindow(CounterConfig *conf, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow),
    conf(conf)
{
    ui->setupUi(this);

    /* Left panel for data display */
    createAllDisplays();
    configureAllDisplays();

    /* Right panel with tabs configured for settings */
    createCounterTabs();
    configureCounterTabs();

    /* Common Counter Signals/Slots */
    connect(tabs, &widgetTab::tabBarClicked, this, &CounterWindow::switchCounterModeCallback);

    /* High Frequency Counter Signals/Slots */
    connect(tabHighFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &CounterWindow::hfSwitchQuantityCallback);
    connect(tabHighFreq->buttonsErrorSwitch, &WidgetButtons::clicked, this, &CounterWindow::hfSwitchErrorAvgCallback);

    /* Low Frequency Counter Signals/Slots */
    connect(tabLowFreq->buttonsChannelSwitch, &WidgetButtons::clicked, this, &CounterWindow::lfSwitchChannelCallback);
    connect(tabLowFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &CounterWindow::lfSwitchQuantityCallback);
    connect(tabLowFreq->buttonsDutyCycleSwitch, &WidgetButtons::clicked, this, &CounterWindow::lfSwitchDutyCycleCallback);
}

CounterWindow::~CounterWindow()
{
    delete ui;
}

void CounterWindow::createCounterTabs(void){
    tabs = new widgetTab(ui->widget_settings, 4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("High Freq", 0);
    tabs->setText("Low Freq", 1);
    tabs->setText("Freq Ratio", 2);
    tabs->setText("Intervals", 3);
}

void CounterWindow::configureCounterTabs(void){
    tabHighFreq = new CounterTabHighFreq(tabs->getLayout(0), tabs);
    tabLowFreq = new CounterTabLowFreq(tabs->getLayout(1), tabs);
    tabRatio = new CounterTabRatio(tabs->getLayout(2), tabs);
    tabInter = new CounterTabIntervals(tabs->getLayout(3), tabs);
}

void CounterWindow::createAllDisplays(void){
    displayHF = createHighFreqDisplay();
    displayHF->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayHF);

    displayLFCh1 = createLowFreqDisplays("LowFreqCh1Counter");
    displayLFCh1->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayLFCh1);

    displayLFCh2 = createLowFreqDisplays("LowFreqCh2Counter");
    displayLFCh2->setContentsMargins(5, 0, 5, 5);
    QString style = "QProgressBar {border: 1px solid #777;border-radius: 1px;background: "+QString::fromUtf8(BACKGROUND_COLOR_DATA_AREA)+"}"
                    "QProgressBar::chunk {background-color: "+QString::fromUtf8(COLOR_GREY)+"width: 20px;}";
    displayLFCh2->setBarStyle(style);
    ui->verticalLayout_display->addWidget(displayLFCh2);

    displayRat = createRatioDisplay();
    displayRat->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayRat);

    displayInt = createIntervalsDisplay();
    displayInt->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayInt);
}

WidgetDisplay *CounterWindow::createHighFreqDisplay(void){
    QString styleSheet = IMAGE_UNITS_HZ;
    WidgetDisplay *display  = new WidgetDisplay("HighFreqCounter", LITERAL_FREQUENCY, styleSheet, true, 4, HISTORY_SIZE, this);
    styleSheet = IMAGE_SIGN_AVG;
    display->setAvgStyle(styleSheet);
    configureErrorStyles(display);
    return display;
}

WidgetDisplay *CounterWindow::createLowFreqDisplays(QString name){
    QString styleSheet = IMAGE_UNITS_HZ;
    WidgetDisplay *display = new WidgetDisplay(name, LITERAL_FREQUENCY, styleSheet, true, 3, HISTORY_SIZE, this);
    configureErrorStyles(display);
    display->showAvgDisplay(false);
    return display;
}

WidgetDisplay *CounterWindow::createRatioDisplay(void){
    QString styleSheet = "";
    WidgetDisplay *display  = new WidgetDisplay("RatioCounter", LITERAL_RATIO, styleSheet, false, 1, HISTORY_SIZE, this);
    styleSheet = IMAGE_SIGN_ERR;
    display->setErrStyle(styleSheet);
    styleSheet = IMAGE_SIGN_PLSMNS;
    display->setTerrStyle(styleSheet);
    display->showAvgDisplay(false);
    display->showTerrStyle(false);
    return display;
}

WidgetDisplay *CounterWindow::createIntervalsDisplay(void){
    QString styleSheet = IMAGE_UNITS_SEC;
    WidgetDisplay *display  = new WidgetDisplay("IntervalsCounter", LITERAL_INTERVAL, styleSheet, false, 3, HISTORY_SIZE, this);
    configureErrorStyles(display);
    display->showAvgDisplay(false);
    display->showQerrTerrStyle(false);
    return display;
}

void CounterWindow::configureErrorStyles(WidgetDisplay *display){
    QString styleSheet = IMAGE_SIGN_ERR;
    styleSheet = IMAGE_SIGN_ERR;
    display->setErrStyle(styleSheet);
    styleSheet = IMAGE_SIGN_PLSMNS;
    display->setQerrStyle(styleSheet);
    display->setTerrStyle(styleSheet);
}

void CounterWindow::configureAllDisplays(void){
    configureDisplaysStaticAttr(displayHF, "CH1", LITERAL_FREQUENCY, "color:"+QString::fromUtf8(COLOR_BLUE));
    configureDisplaysStaticAttr(displayLFCh1, "CH1", LITERAL_FREQUENCY, "color:"+QString::fromUtf8(COLOR_BLUE));
    configureDisplaysStaticAttr(displayLFCh2, "CH2", LITERAL_FREQUENCY, "color:"+QString::fromUtf8(COLOR_GREY));
    configureDisplaysStaticAttr(displayRat, "CH3/CH1", LITERAL_RATIO, "color:"+QString::fromUtf8(COLOR_BLUE));
    configureDisplaysStaticAttr(displayInt, "CH1 - CH2", LITERAL_INTERVAL, "color:"+QString::fromUtf8(COLOR_BLUE));
    //displayHF->hide();
    displayRat->hide();
    displayInt->hide();
    displayLFCh1->hide();
    displayLFCh2->hide();
}

void CounterWindow::configureDisplaysStaticAttr(WidgetDisplay *display, QString channel, QString quantity, QString sideLabelsColor){
    display->configLabel(LABELNUM_QUAN, quantity, "color:"+QString::fromUtf8(COLOR_GREY), true);
    display->configLabel(LABELNUM_CHAN, channel, sideLabelsColor, true);
    display->configLabel(LABELNUM_INDIC, "    ", sideLabelsColor, true);
}

void CounterWindow::setSpecification(CounterSpec *spec){
    this->spec = spec;
    configureDisplaysDynamicAttr();
}

void CounterWindow::configureDisplaysDynamicAttr(){
    configureDisplaysDynamicLabels(displayHF, spec->pins.hf_ch1);
    displayHF->setProgressBarRange(spec->hf_min_Tg100ms, spec->hf_max);

    configureDisplaysDynamicLabels(displayLFCh1, spec->pins.lf_ch1);
    displayLFCh1->setProgressBarRange(spec->lf_min, spec->lf_max);

    configureDisplaysDynamicLabels(displayLFCh2, spec->pins.lf_ch2);
    displayLFCh2->setProgressBarRange(spec->lf_min, spec->lf_max);

    configureDisplaysDynamicLabels(displayRat, spec->pins.rat_ref + "/" + spec->pins.rat_ch3);
    configureDisplaysDynamicLabels(displayInt, spec->pins.int_ch1 + " - " + spec->pins.int_ch2);
}

void CounterWindow::configureDisplaysDynamicLabels(WidgetDisplay *display, QString pin){
    display->configLabel(LABELNUM_PINS, pin, "color:"+QString::fromUtf8(COLOR_GREY), true);
    display->configLabel(LABELNUM_FLAG, "", "color:"+QString::fromUtf8(COLOR_ORANGE), true);
}

void CounterWindow::switchCounterModeCallback(int index){
    resetPreviousCounterMode();
    setNextCounterMode(index);
    conf->modePrevIndex = (CounterMode)index;
}

void CounterWindow::resetPreviousCounterMode(){
    displayHF->hide();
    if(conf->modePrevIndex == CounterMode::LOW_FREQUENCY){
        displayLFCh1->hide();
        displayLFCh2->hide();
    }else if(conf->modePrevIndex == CounterMode::RATIO) {
        displayRat->hide();
    }else if(conf->modePrevIndex == CounterMode::INTERVAL) {
        displayInt->hide();
    }
}

void CounterWindow::setNextCounterMode(int index){
    if(index == (int)CounterMode::HIGH_FREQUENCY){
        displayHF->show();
        clearDisplay(displayHF, true);

    }else if(index == (int)CounterMode::LOW_FREQUENCY) {
        if(conf->lfState.chan1.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            displayLFCh1->displayAvgString("");
            clearDisplay(displayLFCh1, true);
            displayLFCh2->setLabelText(LABELNUM_FLAG, LITERAL_NO_DATA);
            clearDisplay(displayLFCh2, false);
        }else if(conf->lfState.chan2.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            displayLFCh2->displayAvgString("");
            clearDisplay(displayLFCh2, true);
            displayLFCh1->setLabelText(LABELNUM_FLAG, LITERAL_NO_DATA);
            clearDisplay(displayLFCh1, false);
        }else {
            clearDisplay(displayLFCh1, true);
            clearDisplay(displayLFCh2, true);
            switchQuantity((int)conf->lfState.chan1.quantity, displayLFCh1);
            switchQuantity((int)conf->lfState.chan2.quantity, displayLFCh2);
        }
        displayLFCh1->show();
        displayLFCh2->show();

    }else if(index == (int)CounterMode::RATIO) {
        displayRat->show();
        clearDisplay(displayRat, true);

    }else if(index == (int)CounterMode::INTERVAL) {
        displayInt->show();
        clearDisplay(displayInt, false);
    }
}

void CounterWindow::clearDisplay(WidgetDisplay *display, bool uponSwitch){
    display->displayString("");
    display->displayQerrString("");
    display->displayTerrString("");
    displayFlagHoldOn(display, uponSwitch);
    showPMErrorSigns(display, false);
}

void CounterWindow::displayFlagHoldOn(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "HOLD ON" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}

void CounterWindow::displayFlagSwitchMode(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "FREQ 2 HI !!!" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}

void CounterWindow::showPMErrorSigns(WidgetDisplay *display, bool visible){
    display->showQerrTerrStyle(visible);
}

void CounterWindow::switchQuantity(int index, WidgetDisplay *display){
    QString unitsStyleSheet;
    if(index == 0){
        display->setLabelText(LABELNUM_QUAN, LITERAL_FREQUENCY);
        unitsStyleSheet = IMAGE_UNITS_HZ;
    }else if (index == 1) {
        display->setLabelText(LABELNUM_QUAN, LITERAL_PERIOD);
        unitsStyleSheet = IMAGE_UNITS_SEC;
    }
    display->setUnitsStyle(unitsStyleSheet);
}

void CounterWindow::appendNewHistorySample(WidgetDisplay *display, QString prefix, float sample, QString affix, float timeStep){
    display->appendNewHistorySample(prefix, sample, affix, timeStep);
}

void CounterWindow::associateToHistorySample(WidgetDisplay *display, int traceIndex, QString prefix, float sample, QString affix){
    display->associateSample(traceIndex, prefix, sample, affix);
}

void CounterWindow::restoreGUIAfterStartup(){
    switchCounterModeCallback((int)conf->mode);
    if(conf->mode == CounterMode::LOW_FREQUENCY){
        lfSwitchChannelCallback((int)conf->lfState.activeChan);
        if(conf->lfState.chan1.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            lfSwitchDutyCycleCallback((int)LFState::Channel::DutyCycle::ENABLED);
        }else if(conf->lfState.chan2.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            lfSwitchDutyCycleCallback((int)LFState::Channel::DutyCycle::ENABLED);
        }
    }
}

/************************************** HIGH FREQ FUNCTIONS ****************************************/

void CounterWindow::hfSwitchQuantityCallback(int index){
    switchQuantity(index, displayHF);
}

void CounterWindow::hfSwitchErrorAvgCallback(int index){
    QString unitsStyleSheet = (index == 0) ? IMAGE_SIGN_ERR : IMAGE_SIGN_ERRAVG;
    displayHF->setErrStyle(unitsStyleSheet);
}

void CounterWindow::hfSetColorRemainSec(QColor color){
    displayHF->changeAvgColor(color);
}

/************************************** LOW FREQ FUNCTIONS ****************************************/

void CounterWindow::lfSwitchChannelCallback(int index){
    QString bckgndColor;

    if(index == 0){
        bckgndColor = "background-color:"+QString::fromUtf8(COLOR_BLUE);
        tabLowFreq->showDialInChannel(CHANNEL_1, true);
        tabLowFreq->showDialInChannel(CHANNEL_2, false);
    }else if (index == 1) {
        bckgndColor = tabLowFreq->lfCh2BckgndColor;
        tabLowFreq->showDialInChannel(CHANNEL_1, false);
        tabLowFreq->showDialInChannel(CHANNEL_2, true);
    }
    tabLowFreq->changeAllComponentsColor(bckgndColor);

    LFState::Channel chanState = (index == 0) ? conf->lfState.chan1 : conf->lfState.chan2;
    tabLowFreq->changeAllComponentsCheck((int)chanState.quantity, (int)chanState.dutyCycle, (int)chanState.multiplier);
}

void CounterWindow::lfSwitchQuantityCallback(int index){
    WidgetDisplay *display = (conf->lfState.activeChan == LFState::ActiveChan::CHAN1) ? displayLFCh1 : displayLFCh2;
    switchQuantity(index, display);
}

void CounterWindow::lfSwitchDutyCycleCallback(int index){
    WidgetDisplay *display, *unavailDisplay;
    //QString unitsStyleSheet;
    QString pin;

    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        display = displayLFCh1;
        unavailDisplay = displayLFCh2;
        pin = spec->pins.lf_ch1;
    }else {
        display = displayLFCh2;
        unavailDisplay = displayLFCh1;
        pin = spec->pins.lf_ch2;
    }

    if(index == (int)LFState::Channel::DutyCycle::DISABLED){
        lfResetDutyCycle(display, unavailDisplay);
    }else {
        lfSetDutyCycle(display, unavailDisplay, pin);
    }
}

void CounterWindow::lfSetDutyCycle(WidgetDisplay *display, WidgetDisplay *unavailDisplay, QString pin){
    tabLowFreq->enableAllComponents(false);

    unavailDisplay->showQerrTerrStyle(false);
    unavailDisplay->setLabelText(LABELNUM_QUAN, "UNAVAILABLE");

    display->setLabelText(LABELNUM_QUAN, "PULSE WIDTH  |  DUTY CYCLE");
    display->setLabelText(LABELNUM_PINS, pin);

    QString unitsStyleSheet = IMAGE_UNITS_SEC;
    display->setAvgStyle(unitsStyleSheet);
    unitsStyleSheet = IMAGE_SIGN_PERCENT;
    display->setUnitsStyle(unitsStyleSheet);
    unitsStyleSheet = "";
    unavailDisplay->setUnitsStyle(unitsStyleSheet);
    unavailDisplay->setErrStyle(unitsStyleSheet);

    showPMErrorSigns(display, false);
    showPMErrorSigns(unavailDisplay, false);

    lfShowDutyCycleDisplays(display, true);
    unavailDisplay->showBarDisplay(false);
    unavailDisplay->setLabelText(LABELNUM_FLAG, LITERAL_NO_DATA);

    unavailDisplay->drawIndicationFlag(LABELNUM_INDIC, "clear");
    clearDisplay(unavailDisplay, false);
    clearDisplay(display, true);
}

void CounterWindow::lfResetDutyCycle(WidgetDisplay *display, WidgetDisplay *unavailDisplay){
    tabLowFreq->enableAllComponents(true);

    unavailDisplay->showQerrTerrStyle(true);

    switchQuantity((int)conf->lfState.chan1.quantity, displayLFCh1);
    switchQuantity((int)conf->lfState.chan2.quantity, displayLFCh2);
    lfShowErrorStyle(unavailDisplay, true);

    lfShowDutyCycleDisplays(display, false);
    unavailDisplay->showBarDisplay(true);

    clearDisplay(unavailDisplay, true);
    clearDisplay(display, true);
}

void CounterWindow::lfShowDutyCycleDisplays(WidgetDisplay *display, bool dutyEnable){
    display->showAvgDisplay(dutyEnable);
    display->showErrDisplay(!dutyEnable);
    display->showBarDisplay(!dutyEnable);
}

void CounterWindow::lfShowErrorStyle(WidgetDisplay *display, bool show){
    QString unitsStyleSheet;
    unitsStyleSheet = (show) ? IMAGE_SIGN_ERR: "";
    display->setErrStyle(unitsStyleSheet);
}

/************************************** RATIO MEAS. FUNCTIONS ****************************************/
void CounterWindow::ratDisplayFlagWarning(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "SC 2 LOW !!!" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}

/************************************ INTERVALS MEAS. FUNCTIONS ***************************************/
void CounterWindow::intDisplayFlagTimeout(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Timeout!" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}
