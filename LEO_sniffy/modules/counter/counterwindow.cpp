#include "counterwindow.h"
#include "ui_counterwindow.h"

CounterWindow::CounterWindow(CounterConfig *conf, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow),
    conf(conf)
{
    ui->setupUi(this);

    /* Left panel for data display */
    createTwoDisplays();
    configureDisplays();

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

void CounterWindow::createTwoDisplays(void){
    displayCh1 = createDisplay();
    displayCh1->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayCh1);
    displayCh2 = createDisplay();
    displayCh2->setContentsMargins(5, 0, 5, 5);
    ui->verticalLayout_display->addWidget(displayCh2);
}

WidgetDisplay *CounterWindow::createDisplay(void){
    QString styleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    WidgetDisplay *display = new WidgetDisplay(LITERAL_FREQUENCY, styleSheet, true, this);

    styleSheet = "image: url(:/graphics/graphics/units_avg.png); border: none;";
    display->setAvgStyle(styleSheet);
    styleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    display->setErrStyle(styleSheet);
    styleSheet = "image: url(:/graphics/graphics/sign_pm.png); border: none;";
    display->setQerrStyle(styleSheet);
    display->setTerrStyle(styleSheet);

    display->displayString("0.00");

    return display;
}

void CounterWindow::configureDisplays(void){    
    displayCh1->configLabel(LABELNUM_QUAN, LITERAL_FREQUENCY, TEXT_COLOR_GREY, true);
    displayCh1->configLabel(LABELNUM_CHAN, "CH1", TEXT_COLOR_BLUE, true);
    displayCh1->configLabel(LABELNUM_FLAG, "    ", TEXT_COLOR_BLUE, true);
    displayCh2->configLabel(LABELNUM_QUAN, LITERAL_FREQUENCY, TEXT_COLOR_GREY, true);
    displayCh2->configLabel(LABELNUM_CHAN, "CH2", TEXT_COLOR_GREY, true);
    displayCh2->configLabel(LABELNUM_FLAG, "    ", TEXT_COLOR_BLUE, true);
    displayCh2->hide();
}

void CounterWindow::specReceived(CounterSpec *spec){
    displayCh1->configLabel(LABELNUM_PINS, spec->pins.hf_pin, TEXT_COLOR_GREY, true);
    displayCh1->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    displayCh1->setRangeProgressBar(spec->hf_lowLmt_Tg100ms, spec->hf_uppLmt);
    displayCh2->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch2, TEXT_COLOR_GREY, true);
    displayCh2->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    displayCh2->setRangeProgressBar(spec->lf_lowLmt, spec->lf_uppLmt);
}

void CounterWindow::switchCounterModeCallback(int index){
    if(index == 0){
        displayCh2->hide();
        displayCh1->showAvgDisplay();
        hfSwitchQuantityCallback((int)conf->hfState.quantity);
        hfSwitchErrorAvgCallback((int)conf->hfState.error);
    }else if(index == 1) {
        displayCh2->show();
        displayCh1->hideAvgDisplay();
        displayCh2->hideAvgDisplay();
        switchQuantity((int)conf->lfState.chan1.quantity, displayCh1);
        hfSwitchErrorAvgCallback(0);
    }else if(index == 2) {
        displayCh2->hide();
        displayCh1->hideAvgDisplay();
    }else if(index == 3) {
        displayCh2->hide();
        displayCh1->hideAvgDisplay();
    }
}

void CounterWindow::displayFlagHoldOn(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Hold on" : "";
    display->setLabelText(LABELNUM_HOLD, string);
}

void CounterWindow::displayFlagSwitchMode(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Switch Mode" : "";
    display->setLabelText(LABELNUM_HOLD, string);
}

void CounterWindow::reconfigDisplayLabelArea(CounterSpec *spec){
    this->spec = spec;
    if(conf->mode == CounterMode::HIGH_FREQUENCY){
        displayCh1->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch1, TEXT_COLOR_GREY, true);
        displayCh1->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    }else if (conf->mode == CounterMode::LOW_FREQUENCY) {
        displayCh2->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch2, TEXT_COLOR_GREY, true);
        displayCh2->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    }else if (conf->mode == CounterMode::REFERENCE) {

    }else if (conf->mode == CounterMode::INTERVAL) {

    }
}

void CounterWindow::showPMErrorSigns(WidgetDisplay *display, bool visible){
    display->showQerrTerrStyle(visible);
}

void CounterWindow::switchQuantity(int index, WidgetDisplay *display){
    QString unitsStyleSheet;
    if(index == 0){
        display->setLabelText(LABELNUM_QUAN, LITERAL_FREQUENCY);
        unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    }else if (index == 1) {
        display->setLabelText(LABELNUM_QUAN, LITERAL_PERIOD);
        unitsStyleSheet = "image: url(:/graphics/graphics/units_sec.png); border: none;";
    }
    display->setUnitsStyle(unitsStyleSheet);
}

/************************************** HIGH FREQ FUNCTIONS ****************************************/

void CounterWindow::hfSwitchQuantityCallback(int index){
    switchQuantity(index, displayCh1);
}

void CounterWindow::hfSwitchErrorAvgCallback(int index){
    QString unitsStyleSheet;
    if(index == 0){
        unitsStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    }else {
        unitsStyleSheet = "image: url(:/graphics/graphics/units_erravg.png); border: none;";
    }
    displayCh1->setErrStyle(unitsStyleSheet);
}

void CounterWindow::hfSetColorRemainSec(QColor color){
    displayCh1->changeAvgColor(color);
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
    WidgetDisplay *display = (conf->lfState.activeChan == LFState::ActiveChan::CHAN1) ? displayCh1 : displayCh2;
    switchQuantity(index, display);
}

void CounterWindow::lfSwitchDutyCycleCallback(int index){
    WidgetDisplay *display, *norDisplay; QString pin; QString unitsStyleSheet;
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        display = displayCh1;
        norDisplay = displayCh2;
        pin = spec->pins.lf_pin_ch1;
    }else {
        display = displayCh2;
        norDisplay = displayCh1;
        pin = spec->pins.lf_pin_ch2;
    }

    if(index == 0){
        lfEnableButtons();
        norDisplay->show();
        displayFlagHoldOn(display, true);
        displayFlagHoldOn(norDisplay, true);
        switchQuantity((int)conf->lfState.chan1.quantity, display);
        switchQuantity((int)conf->lfState.chan2.quantity, norDisplay);
        showPMErrorSigns(display, true);
        display->hideAvgDisplay();
        display->showErrDisplay();
        display->showBarDisplay();
    }else {
        lfDisableButtons();
        norDisplay->hide();
        displayFlagHoldOn(display, true);
        display->setLabelText(LABELNUM_QUAN, "DUTY CYCLE & PULSE WIDTH");
        display->configLabel(LABELNUM_PINS, pin, TEXT_COLOR_GREY, true);
        unitsStyleSheet = "image: url(:/graphics/graphics/units_sec.png); border: none;";
        display->setAvgStyle(unitsStyleSheet);
        unitsStyleSheet = "image: url(:/graphics/graphics/units_perc.png); border: none;";
        display->setUnitsStyle(unitsStyleSheet);
        display->showAvgDisplay();
        showPMErrorSigns(display, false);
        display->hideErrDisplay();
        display->hideBarDisplay();
    }
}

void CounterWindow::lfDisableButtons(){
    tabLowFreq->buttonsChannelSwitch->disableAll();
    tabLowFreq->buttonsQuantitySwitch->disableAll();
    tabLowFreq->buttonsMultiplierSwitch->disableAll();
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        tabLowFreq->dialSampleCountCh1->setEnabled(false);
    } else {
        tabLowFreq->dialSampleCountCh2->setEnabled(false);
    }
}

void CounterWindow::lfEnableButtons(){
    tabLowFreq->buttonsChannelSwitch->enableAll();
    tabLowFreq->buttonsQuantitySwitch->enableAll();
    tabLowFreq->buttonsMultiplierSwitch->enableAll();
    if(conf->lfState.activeChan == LFState::ActiveChan::CHAN1){
        tabLowFreq->dialSampleCountCh1->setEnabled(true);
    }    else {
        tabLowFreq->dialSampleCountCh2->setEnabled(true);
    }
}

/************************************** REFERENCE MEAS. FUNCTIONS ****************************************/
