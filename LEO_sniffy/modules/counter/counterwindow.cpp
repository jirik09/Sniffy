#include "counterwindow.h"
#include "ui_counterwindow.h"

CounterWindow::CounterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow)
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
    displayCh2->configLabel(LABELNUM_CHAN, "CH2", TEXT_COLOR_GREEN, true);
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
    }else if(index == 1) {
        displayCh2->show();
        displayCh1->hideAvgDisplay();
        displayCh2->hideAvgDisplay();
    }else if(index == 2) {
        displayCh1->hideAvgDisplay();
    }else if(index == 3) {
        displayCh1->hideAvgDisplay();
    }
}

void CounterWindow::displayHoldOnFlag(WidgetDisplay *display, bool visible){
    QString string = (visible) ? "Hold on" : "";
    display->setLabelText(LABELNUM_HOLD, string);
}

void CounterWindow::reconfigDisplayLabelArea(CounterConfig* config, CounterSpec *spec){
    if(config->mode == CounterMode::HIGH_FREQUENCY){
        displayCh1->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch1, TEXT_COLOR_GREY, true);
        displayCh1->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    }else if (config->mode == CounterMode::LOW_FREQUENCY) {
        displayCh2->configLabel(LABELNUM_PINS, spec->pins.lf_pin_ch2, TEXT_COLOR_GREY, true);
        displayCh2->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    }else if (config->mode == CounterMode::REFERENCE) {

    }else if (config->mode == CounterMode::INTERVAL) {

    }
}

void CounterWindow::showPMErrorSigns(WidgetDisplay *display, bool visible){
    display->showQerrTerrStyle(visible);
}

/************************************** HIGH FREQ FUNCTIONS ****************************************/

void CounterWindow::hfSwitchQuantityCallback(int index){
    QString unitsStyleSheet;
    if(index == 0){
        displayCh1->setLabelText(LABELNUM_QUAN, LITERAL_FREQUENCY);
        unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    }else {
        displayCh1->setLabelText(LABELNUM_QUAN, LITERAL_PERIOD);
        unitsStyleSheet = "image: url(:/graphics/graphics/units_sec.png); border: none;";
    }
    displayCh1->setUnitsStyle(unitsStyleSheet);
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
    QString bckgndColor = (index == 0) ? BCKGRND_COLOR_BLUE : BCKGRND_COLOR_GREEN;
    tabLowFreq->buttonsQuantitySwitch->setColor(bckgndColor, 0);
    tabLowFreq->buttonsQuantitySwitch->setColor(bckgndColor, 1);
    tabLowFreq->buttonsSampleCountMultiplierSwitch->setColor(bckgndColor, 0);
    tabLowFreq->buttonsSampleCountMultiplierSwitch->setColor(bckgndColor, 1);
    tabLowFreq->buttonsSampleCountMultiplierSwitch->setColor(bckgndColor, 2);
    tabLowFreq->buttonsSampleCountMultiplierSwitch->setColor(bckgndColor, 3);
    tabLowFreq->buttonsDutyCycleSwitch->setColor(bckgndColor, 0);
    tabLowFreq->buttonsDutyCycleSwitch->setColor(bckgndColor, 1);
}
