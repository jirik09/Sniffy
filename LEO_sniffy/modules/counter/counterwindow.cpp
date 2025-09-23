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
    tabInter = new CounterTabIntervals(tabs->getLayout(3), conf, tabs);
}

void CounterWindow::createAllDisplays(void){
    QString chan1 = Graphics::getChannelColor(CHANNEL_1);
    QString chan2 = Graphics::getChannelColor(CHANNEL_2);

    displayHF = createHighFreqDisplay();
    displayHF->setContentsMargins(5, 5, 5, 5);
    displayHF->setIndicationFlagColor(chan1);
    displayHF->setProgressColor(chan1);
    displayHF->useVerticalProgressBar(false);
    ui->verticalLayout_display->addWidget(displayHF);    

    displayLFCh1 = createLowFreqDisplays("LowFreqCh1Counter");
    displayLFCh1->setContentsMargins(5, 5, 5, 5);
    displayLFCh1->setIndicationFlagColor(chan1);
    displayLFCh1->setProgressColor(chan1);
    displayLFCh1->useVerticalProgressBar(false);
    ui->verticalLayout_display->addWidget(displayLFCh1);

    displayLFCh2 = createLowFreqDisplays("LowFreqCh2Counter");
    displayLFCh2->setContentsMargins(5, 0, 5, 5);
    displayLFCh2->setIndicationFlagColor(chan2);
    displayLFCh2->setProgressColor(chan2);
    displayLFCh2->useVerticalProgressBar(false);
    ui->verticalLayout_display->addWidget(displayLFCh2);

    displayRat = createRatioDisplay();
    displayRat->setContentsMargins(5, 5, 5, 5);
    displayRat->setIndicationFlagColor(chan1);
    ui->verticalLayout_display->addWidget(displayRat);

    displayInt = createIntervalsDisplay();
    displayInt->setContentsMargins(5, 5, 5, 5);
    displayInt->setIndicationFlagColor(chan1);
    ui->verticalLayout_display->addWidget(displayInt);
}

WidgetDisplay *CounterWindow::createHighFreqDisplay(void){
    QString styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_hz.png); border: none;";
    WidgetDisplay *display  = new WidgetDisplay("HighFreqCounter", LITERAL_FREQUENCY, styleSheet, true, 4, HISTORY_SIZE, this);
    styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_avg.png); border: none;";
    display->setAvgStyle(styleSheet);
    configureErrorStyles(display);
    return display;
}

WidgetDisplay *CounterWindow::createLowFreqDisplays(QString name){
    QString styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_hz.png); border: none;";
    WidgetDisplay *display = new WidgetDisplay(name, LITERAL_FREQUENCY, styleSheet, true, 3, HISTORY_SIZE, this);
    configureErrorStyles(display);
    display->showAvgDisplay(false);
    return display;
}

WidgetDisplay *CounterWindow::createRatioDisplay(void){
    QString styleSheet = "";
    WidgetDisplay *display  = new WidgetDisplay("RatioCounter", LITERAL_RATIO, styleSheet, false, 1, HISTORY_SIZE, this);
    styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_err.png); border: none;";
    display->setErrStyle(styleSheet);
    styleSheet = "image: url("+Graphics::getGraphicsPath()+"sign_pm.png); border: none;";
    display->setTerrStyle(styleSheet);
    display->showAvgDisplay(false);
    display->showTerrStyle(false);
    return display;
}

WidgetDisplay *CounterWindow::createIntervalsDisplay(void){
    QString styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_sec.png); border: none;";
    WidgetDisplay *display  = new WidgetDisplay("IntervalsCounter", LITERAL_INTERVAL, styleSheet, false, 3, HISTORY_SIZE, this);
    configureErrorStyles(display);
    display->showAvgDisplay(false);
    display->showQerrTerrStyle(false);
    return display;
}

void CounterWindow::configureErrorStyles(WidgetDisplay *display){
    QString styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_err.png); border: none;";
    styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_err.png); border: none;";
    display->setErrStyle(styleSheet);
    styleSheet = "image: url("+Graphics::getGraphicsPath()+"sign_pm.png); border: none;";
    display->setQerrStyle(styleSheet);
    display->setTerrStyle(styleSheet);
}

void CounterWindow::configureAllDisplays(void){
    configureDisplaysStaticAttr(displayHF, "CH1", LITERAL_FREQUENCY, Graphics::getChannelColor(CHANNEL_1));
    configureDisplaysStaticAttr(displayLFCh1, "CH1", LITERAL_FREQUENCY, Graphics::getChannelColor(CHANNEL_1));
    configureDisplaysStaticAttr(displayLFCh2, "CH2", LITERAL_FREQUENCY, Graphics::getChannelColor(CHANNEL_2));
    configureDisplaysStaticAttr(displayRat, "CH3/CH1", LITERAL_RATIO, Graphics::getChannelColor(CHANNEL_1));
    configureDisplaysStaticAttr(displayInt, "CH1 - CH2", LITERAL_INTERVAL, Graphics::getChannelColor(CHANNEL_1));
    displayRat->hide();
    displayInt->hide();
    displayLFCh1->hide();
    displayLFCh2->hide();
}

void CounterWindow::configureDisplaysStaticAttr(WidgetDisplay *display, QString channel, QString quantity, QString sideLabelsColor){
    display->configLabel(LABELNUM_QUAN, quantity, Graphics::palette().textLabel, true);
    display->configLabel(LABELNUM_CHAN, channel, sideLabelsColor, true);
    display->configLabel(LABELNUM_INDIC, "    ", sideLabelsColor, true);
}

void CounterWindow::setSpecification(CounterSpec *spec){
    this->spec = spec;
    configureDisplaysDynamicAttr();
}

void CounterWindow::configureDisplaysDynamicAttr(){
    configureDisplaysDynamicLabels(displayHF, spec->pins.hf_ch1, CHANNEL_1);
    // Start range at 0 to guarantee visual empty state when frequency drops
    displayHF->setProgressRange(0, spec->hf_max);

    configureDisplaysDynamicLabels(displayLFCh1, spec->pins.lf_ch1, CHANNEL_1);
    displayLFCh1->setProgressRange(0, spec->lf_max);

    configureDisplaysDynamicLabels(displayLFCh2, spec->pins.lf_ch2, CHANNEL_2);
    displayLFCh2->setProgressRange(0, spec->lf_max);

    configureDisplaysDynamicLabels(displayRat, spec->pins.rat_ref + "/" + spec->pins.rat_ch3, CHANNEL_1);
    configureDisplaysDynamicLabels(displayInt, spec->pins.int_ch1 + " - " + spec->pins.int_ch2, CHANNEL_1);
}

void CounterWindow::configureDisplaysDynamicLabels(WidgetDisplay *display, QString pin, int chan){
    display->configLabel(LABELNUM_PINS, pin, Graphics::getChannelColor(chan), true);
    display->configLabel(LABELNUM_FLAG, "", Graphics::palette().warning, true);
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
        tabInter->setStartButton(false);
    }
}

void CounterWindow::setNextCounterMode(int index){
    if(index == (int)CounterMode::HIGH_FREQUENCY){
        displayHF->show();
        clearDisplay(displayHF, true);

    }else if(index == (int)CounterMode::LOW_FREQUENCY) {
        if(isAfterStart == true)
            lfSwitchChannelCallback((int)conf->lfState.activeChan);
        if(conf->lfState.chan1.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            if(isAfterStart == true)
                lfSwitchDutyCycleCallback((int)LFState::Channel::DutyCycle::ENABLED);
            displayLFCh1->displayAvgString("");
            clearDisplay(displayLFCh1, true);
            displayLFCh2->setLabelText(LABELNUM_FLAG, LITERAL_NO_DATA);
            clearDisplay(displayLFCh2, false);
        }else if(conf->lfState.chan2.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            if(isAfterStart == true)
                lfSwitchDutyCycleCallback((int)LFState::Channel::DutyCycle::ENABLED);
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
        if(isAfterStart == true)
            isAfterStart = false;
        displayLFCh1->show();
        displayLFCh2->show();

    }else if(index == (int)CounterMode::RATIO) {
        displayRat->show();
        clearDisplay(displayRat, false);

    }else if(index == (int)CounterMode::INTERVAL) {
        tabInter->loadGraphics();
        displayInt->show();
        clearDisplay(displayInt, false);
    }
}

void CounterWindow::clearDisplay(WidgetDisplay *display, bool uponSwitch){
    display->displayString("");
    display->displayQerrString("");
    display->displayTerrString("");
    displayFlagAcquiring(display, uponSwitch);
    showPMErrorSigns(display, false);
}

void CounterWindow::displayFlagAcquiring(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Acquiring" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}

void CounterWindow::displayFlagSwitchMode(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Freq 2 hi !!!" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}

void CounterWindow::showPMErrorSigns(WidgetDisplay *display, bool visible){
    display->showQerrTerrStyle(visible);
}

void CounterWindow::switchQuantity(int index, WidgetDisplay *display){
    QString unitsStyleSheet;
    if(index == 0){
        display->setLabelText(LABELNUM_QUAN, LITERAL_FREQUENCY);
        unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_hz.png); border: none;";
    }else if (index == 1) {
        display->setLabelText(LABELNUM_QUAN, LITERAL_PERIOD);
        unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_sec.png); border: none;";
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
}

/************************************** HIGH FREQ FUNCTIONS ****************************************/

void CounterWindow::hfSwitchQuantityCallback(int index){
    switchQuantity(index, displayHF);
}

void CounterWindow::hfSwitchErrorAvgCallback(int index){
    QString unitsStyleSheet;
    if(index == 0)
        unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_err.png); border: none;";
    else
        unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_erravg.png); border: none;";

    displayHF->setErrStyle(unitsStyleSheet);
}

void CounterWindow::hfSetColorRemainSec(bool waiting){
    QColor color = (waiting) ? Graphics::palette().textLabel : Graphics::palette().textAll;
    displayHF->changeAvgColor(color);
}

/************************************** LOW FREQ FUNCTIONS ****************************************/

void CounterWindow::lfSwitchChannelCallback(int index){
    QString bckgndColor;

    if(index == CHANNEL_1){
        bckgndColor = Graphics::getChannelColor(CHANNEL_1);
        tabLowFreq->showDialInChannel(CHANNEL_1, true);
        tabLowFreq->showDialInChannel(CHANNEL_2, false);
    }else if (index == CHANNEL_2) {
        bckgndColor = Graphics::getChannelColor(CHANNEL_2);
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

    QString unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_sec.png); border: none;";
    display->setAvgStyle(unitsStyleSheet);
    unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_perc.png); border: none;";
    display->setUnitsStyle(unitsStyleSheet);
    unitsStyleSheet = "";
    unavailDisplay->setUnitsStyle(unitsStyleSheet);
    unavailDisplay->setErrStyle(unitsStyleSheet);

    showPMErrorSigns(display, false);
    showPMErrorSigns(unavailDisplay, false);

    lfShowDutyCycleDisplays(display, true);
    unavailDisplay->showBarDisplay(false);
    unavailDisplay->setLabelText(LABELNUM_FLAG, LITERAL_NO_DATA);

    unavailDisplay->clearIndicationFlag(LABELNUM_INDIC);
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
    if(show)
        unitsStyleSheet = "image: url("+Graphics::getGraphicsPath()+"units_err.png); border: none;";
    else
        unitsStyleSheet = "";

    display->setErrStyle(unitsStyleSheet);
}

/************************************** RATIO MEAS. FUNCTIONS ****************************************/
void CounterWindow::ratDisplayFlagWarning(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "SC 2 low !!!" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}

/************************************ INTERVALS MEAS. FUNCTIONS ***************************************/
void CounterWindow::intDisplayFlagTimeout(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Timeout!" : "";
    display->setLabelText(LABELNUM_FLAG, string);
}
