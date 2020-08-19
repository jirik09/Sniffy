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
}

void CounterWindow::createAllDisplays(void){
    QString styleSheet = IMAGE_UNITS_HZ;
    displayHF = new WidgetDisplay(LITERAL_FREQUENCY, styleSheet, true, this);
    displayHF->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayHF);
    styleSheet = IMAGE_SIGN_AVG;
    displayHF->setAvgStyle(styleSheet);
    configureHFLFErrorStyles(displayHF);

    displayLFCh1 = createLFDisplays();
    displayLFCh1->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayLFCh1);
    displayLFCh2 = createLFDisplays();
    displayLFCh2->setContentsMargins(5, 0, 5, 5);
    ui->verticalLayout_display->addWidget(displayLFCh2);
}

WidgetDisplay *CounterWindow::createLFDisplays(void){
    QString styleSheet = IMAGE_UNITS_HZ;
    WidgetDisplay *display = new WidgetDisplay(LITERAL_FREQUENCY, styleSheet, true, this);
    display->showAvgDisplay(false);
    configureHFLFErrorStyles(display);
    return display;
}

void CounterWindow::configureHFLFErrorStyles(WidgetDisplay *display){
    QString styleSheet = IMAGE_SIGN_ERR;
    styleSheet = IMAGE_SIGN_ERR;
    display->setErrStyle(styleSheet);
    styleSheet = IMAGE_SIGN_PLSMNS;
    display->setQerrStyle(styleSheet);
    display->setTerrStyle(styleSheet);
    display->displayString("");
}

void CounterWindow::configureAllDisplays(void){
    configureDisplaysStaticAttr(displayHF, "CH1", TEXT_COLOR_BLUE);
    configureDisplaysStaticAttr(displayLFCh1, "CH1", TEXT_COLOR_BLUE);
    configureDisplaysStaticAttr(displayLFCh2, "CH2", TEXT_COLOR_GREY);

    displayLFCh1->hide();
    displayLFCh2->hide();
}

void CounterWindow::configureDisplaysStaticAttr(WidgetDisplay *display, QString channel, QString sideLabelsColor){
    display->configLabel(LABELNUM_QUAN, LITERAL_FREQUENCY, TEXT_COLOR_GREY, true);
    display->configLabel(LABELNUM_CHAN, channel, sideLabelsColor, true);
    display->configLabel(LABELNUM_FLAG, "    ", sideLabelsColor, true);
}

void CounterWindow::setSpecification(CounterSpec *spec){
    this->spec = spec;
    configureDisplaysDynamicAttr();
}

void CounterWindow::configureDisplaysDynamicAttr(){
    displayHF->configLabel(LABELNUM_PINS, spec->pins.hf_pin, TEXT_COLOR_GREY, true);
    displayHF->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    displayHF->setRangeProgressBar(spec->hf_lowLmt_Tg100ms, spec->hf_uppLmt);

    displayLFCh1->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch1, TEXT_COLOR_GREY, true);
    displayLFCh1->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    displayLFCh1->setRangeProgressBar(spec->lf_lowLmt, spec->hf_uppLmt);

    displayLFCh2->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch2, TEXT_COLOR_GREY, true);
    displayLFCh2->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    displayLFCh2->setRangeProgressBar(spec->lf_lowLmt, spec->lf_uppLmt);
}

void CounterWindow::switchCounterModeCallback(int index){
    resetPreviousCounterMode();
    setNextCounterMode(index);
    modePrevIndex = index;
}

void CounterWindow::resetPreviousCounterMode(){
    if(modePrevIndex == (int)CounterMode::HIGH_FREQUENCY){
        displayHF->hide();
        clearDisplay(displayHF);
    }else if(modePrevIndex == (int)CounterMode::LOW_FREQUENCY){
        displayLFCh1->hide();
        displayLFCh2->hide();
        clearDisplay(displayLFCh1);
        clearDisplay(displayLFCh2);
    }else if(modePrevIndex == (int)CounterMode::REFERENCE) {
        displayRef->hide();
        clearDisplay(displayRef);
    }else if(modePrevIndex == (int)CounterMode::INTERVAL) {
        displayInt->hide();
        clearDisplay(displayRef);
    }
}

void CounterWindow::setNextCounterMode(int index){
    if(index == (int)CounterMode::HIGH_FREQUENCY){
        displayFlagHoldOn(displayHF, true);
        displayHF->show();
    }else if(index == (int)CounterMode::LOW_FREQUENCY) {
        if(conf->lfState.chan1.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            displayFlagHoldOn(displayLFCh1, true);
            displayLFCh1->show();
            displayLFCh2->hide();
        }else if(conf->lfState.chan2.dutyCycle == LFState::Channel::DutyCycle::ENABLED){
            displayFlagHoldOn(displayLFCh2, true);
            displayLFCh2->show();
            displayLFCh1->hide();
        }else {
            displayFlagHoldOn(displayLFCh1, true);
            displayFlagHoldOn(displayLFCh2, true);
            displayLFCh1->show();
            displayLFCh2->show();
        }
    }else if(index == (int)CounterMode::REFERENCE) {
        displayFlagHoldOn(displayRef, true);
        displayRef->show();
    }else if(index == (int)CounterMode::INTERVAL) {
        displayFlagHoldOn(displayInt, true);
        displayInt->show();
    }
}

void CounterWindow::clearDisplay(WidgetDisplay *display){
    if(conf->mode != CounterMode::HIGH_FREQUENCY && conf->hfState.hold == HFState::HoldOnState::OFF)
        display->displayAvgString("");
    display->displayString("");
    display->displayQerrString("");
    display->displayTerrString("");
    displayFlagHoldOn(display, true);
    showPMErrorSigns(display, false);
}

void CounterWindow::displayFlagHoldOn(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Hold on" : "";
    display->setLabelText(LABELNUM_HOLD, string);
}

void CounterWindow::displayFlagSwitchMode(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Switch Mode" : "";
    display->setLabelText(LABELNUM_HOLD, string);
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

void CounterWindow::msleep(int msec){
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, &QEventLoop::quit);
    loop.exec();
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
        bckgndColor = BCKGRND_COLOR_BLUE;
        tabLowFreq->dialSampleCountCh2->hide();
        tabLowFreq->dialSampleCountCh1->show();
    }else if (index == 1) {
        bckgndColor = tabLowFreq->lfCh2BckgndColor;
        tabLowFreq->dialSampleCountCh1->hide();
        tabLowFreq->dialSampleCountCh2->show();
    }

    tabLowFreq->buttonsQuantitySwitch->setColor(bckgndColor, 0);
    tabLowFreq->buttonsQuantitySwitch->setColor(bckgndColor, 1);
    tabLowFreq->buttonsMultiplierSwitch->setColor(bckgndColor, 0);
    tabLowFreq->buttonsMultiplierSwitch->setColor(bckgndColor, 1);
    tabLowFreq->buttonsMultiplierSwitch->setColor(bckgndColor, 2);
    tabLowFreq->buttonsMultiplierSwitch->setColor(bckgndColor, 3);
    tabLowFreq->buttonsDutyCycleSwitch->setColor(bckgndColor, 0);
    tabLowFreq->buttonsDutyCycleSwitch->setColor(bckgndColor, 1);
}

void CounterWindow::lfSwitchQuantityCallback(int index){    
    WidgetDisplay *display = (conf->lfState.activeChan == LFState::ActiveChan::CHAN1) ? displayLFCh1 : displayLFCh2;
    switchQuantity(index, display);
}

void CounterWindow::lfSwitchDutyCycleCallback(int index){
    WidgetDisplay *display, *norDisplay;
    QString unitsStyleSheet;
    QString pin;

    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        display = displayLFCh1;
        norDisplay = displayLFCh2;
        pin = spec->pins.lf_pin_ch1;
        clearDisplay(displayLFCh1);
    }else {
        display = displayLFCh2;
        norDisplay = displayLFCh1;
        pin = spec->pins.lf_pin_ch2;
        clearDisplay(displayLFCh2);
    }

    if(index == (int)LFState::Channel::DutyCycle::DISABLED){
        lfResetDutyCycle(display, norDisplay);
    }else {
        lfSetDutyCycle(display, norDisplay, pin);
    }
}

void CounterWindow::lfSetDutyCycle(WidgetDisplay *display, WidgetDisplay *norDisplay, QString pin){
    lfEnableTabsComponents(false);
    norDisplay->hide();

    display->setLabelText(LABELNUM_QUAN, "PULSE WIDTH  |  DUTY CYCLE");
    display->setLabelText(LABELNUM_PINS, pin);

    QString unitsStyleSheet;
    unitsStyleSheet = IMAGE_UNITS_SEC;
    display->setAvgStyle(unitsStyleSheet);
    unitsStyleSheet = IMAGE_SIGN_PERCENT;
    display->setUnitsStyle(unitsStyleSheet);

    displayFlagHoldOn(display, true);
    showPMErrorSigns(display, false);

    lfShowDutyCycleDisplays(display, true);
}

void CounterWindow::lfResetDutyCycle(WidgetDisplay *display, WidgetDisplay *norDisplay){
    lfEnableTabsComponents(true);
    norDisplay->show();

    switchQuantity((int)conf->lfState.chan1.quantity, display);
    switchQuantity((int)conf->lfState.chan2.quantity, norDisplay);

    displayFlagHoldOn(display, true);
    displayFlagHoldOn(norDisplay, true);
    showPMErrorSigns(display, true);

    lfShowDutyCycleDisplays(display, false);
}

void CounterWindow::lfShowDutyCycleDisplays(WidgetDisplay *display, bool dutyEnable){
    display->showAvgDisplay(dutyEnable);
    display->showErrDisplay(!dutyEnable);
    display->showBarDisplay(!dutyEnable);
}

void CounterWindow::lfEnableTabsComponents(bool enable){
    tabLowFreq->buttonsChannelSwitch->enableAll(enable);
    tabLowFreq->buttonsQuantitySwitch->enableAll(enable);
    tabLowFreq->buttonsMultiplierSwitch->enableAll(enable);
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        tabLowFreq->dialSampleCountCh1->setEnabled(enable);
    }    else {
        tabLowFreq->dialSampleCountCh2->setEnabled(enable);
    }
}

/************************************** REFERENCE MEAS. FUNCTIONS ****************************************/
