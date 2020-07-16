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
    connect(tabHighFreq->buttonsQuantitySwitch, &WidgetButtons::clicked, this, &CounterWindow::switchFrequencyPeriodCallback);
    connect(tabHighFreq->buttonsErrorSwitch, &WidgetButtons::clicked, this, &CounterWindow::switchErrorAvgCallback);
}

CounterWindow::~CounterWindow()
{
    delete ui;
}

void CounterWindow::createCounterTabs(void){
    tabs = new widgetTab(ui->widget_settings,4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("High Freq",0);
    tabs->setText("Low Freq",1);
    tabs->setText("Freq Ratio",2);
    tabs->setText("Intervals",3);
}

void CounterWindow::configureCounterTabs(void){
    tabHighFreq = new CounterTabHighFreq(tabs->getLayout(0), tabs);
    //tabLowFreq = new CounterTabLowFreq(tabs->getLayout(1), tabs);
}

void CounterWindow::createTwoDisplays(void){
    displayChannel1 = createDisplay();
    displayChannel1->setContentsMargins(5, 5, 5, 5);
    ui->verticalLayout_display->addWidget(displayChannel1);
    displayChannel2 = createDisplay();
    displayChannel2->setContentsMargins(5, 0, 5, 5);
    ui->verticalLayout_display->addWidget(displayChannel2);
}

WidgetDisplay *CounterWindow::createDisplay(void){
    QString unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    WidgetDisplay *display = new WidgetDisplay("Frequency", unitsStyleSheet, true, this);
    QString avgStyleSheet = "image: url(:/graphics/graphics/units_avg.png); border: none;";
    display->setAvgStyle(avgStyleSheet);
    QString errStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    display->setErrStyle(errStyleSheet);
    display->displayNumber("0.00");

    return display;
}

void CounterWindow::configureDisplays(void){
    displayChannel1->configLabel(4, "CH1", "green", false);
    displayChannel2->configLabel(4, "CH2", "violet", true);
    displayChannel2->hide();
}

WidgetDisplay *CounterWindow::getDisplayChannel1(){
    return displayChannel1;
}

WidgetDisplay *CounterWindow::getDisplayChannel2(){
    return displayChannel2;
}

void CounterWindow::switchCounterModeCallback(int index){
    if(index == 1){
        displayChannel2->show();
        displayChannel1->showLabel(4);
    }else {
        displayChannel2->hide();
        displayChannel1->hideLabel(4);
    }
}

void CounterWindow::switchFrequencyPeriodCallback(int index){
    QString unitsStyleSheet;
    if(index == 0){
        displayChannel1->setLabelText(0, "Frequency");
        unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
        displayChannel1->setUnitsStyle(unitsStyleSheet);
    }else {
        displayChannel1->setLabelText(0, "Period");
        unitsStyleSheet = "image: url(:/graphics/graphics/units_sec.png); border: none;";
        displayChannel1->setUnitsStyle(unitsStyleSheet);
    }
}

void CounterWindow::switchErrorAvgCallback(int index){
    QString unitsStyleSheet;
    if(index == 0){
        unitsStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
        displayChannel1->setErrStyle(unitsStyleSheet);
    }else {
        unitsStyleSheet = "image: url(:/graphics/graphics/units_erravg.png); border: none;";
        displayChannel1->setErrStyle(unitsStyleSheet);
    }
}


