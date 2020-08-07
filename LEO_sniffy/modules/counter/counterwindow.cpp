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

    connect(tabs, &widgetTab::tabBarClicked, this, &CounterWindow::switchCounterModeCallback);
    connect(tabHighFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &CounterWindow::switchQuantityCallback);
    connect(tabHighFreq->buttonsErrorSwitch, &WidgetButtons::clicked, this, &CounterWindow::switchErrorAvgCallback);
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
    //tabLowFreq = new CounterTabLowFreq(tabs->getLayout(1), tabs);
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
    displayCh1->configLabel(LABELNUM_CHAN, "CH1", TEXT_COLOR_GREY, true);
    displayCh1->configLabel(LABELNUM_FLAG, "    ", TEXT_COLOR_BLUE, true);

    displayCh1->configLabel(LABELNUM_QUAN, LITERAL_FREQUENCY, TEXT_COLOR_GREY, true);
    displayCh2->configLabel(LABELNUM_CHAN, "CH2", TEXT_COLOR_GREY, true);
    displayCh2->configLabel(LABELNUM_FLAG, "    ", TEXT_COLOR_BLUE, true);
    displayCh2->hide();
}

void CounterWindow::switchCounterModeCallback(int index){
    if(index == 1){
        displayCh2->show();
    }else {
        displayCh2->hide();
    }
}

void CounterWindow::switchQuantityCallback(int index){
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

void CounterWindow::switchErrorAvgCallback(int index){
    QString unitsStyleSheet;
    if(index == 0){
        unitsStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    }else {
        unitsStyleSheet = "image: url(:/graphics/graphics/units_erravg.png); border: none;";
    }
    displayCh1->setErrStyle(unitsStyleSheet);
}

void CounterWindow::setColorHoldOn(QColor color){
    displayCh1->changeAvgColor(color);
}

void CounterWindow::displayHoldOnFlag(bool visible){
    if(visible){
        displayCh1->setLabelText(LABELNUM_HOLD, "Hold on");
    }else {
        displayCh1->setLabelText(LABELNUM_HOLD, "");
    }
}

void CounterWindow::specReceived(CounterSpec *spec){
    displayCh1->configLabel(LABELNUM_PINS, spec->pins.hf_pin, TEXT_COLOR_GREY, true);
    displayCh1->configLabel(LABELNUM_HOLD, "", TEXT_COLOR_ORANGE, true);
    displayCh1->setRangeProgressBar(spec->hf_lowLmt_Tg100ms, spec->hf_uppLmt);
    displayCh2->setRangeProgressBar(spec->lf_lowLmt, spec->lf_uppLmt);
}
