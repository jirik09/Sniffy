#include "counterwindow.h"
#include "ui_counterwindow.h"

CounterWindow::CounterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);

    createCounterTabs();
    createHighFrequencyDisplay();
    createLowFrequencyDisplay();
}

CounterWindow::~CounterWindow()
{
    delete ui;
}

void CounterWindow::createCounterTabs(void){
    tabs = new widgetTab(ui->widget_display,4);
    ui->verticalLayout_display->addWidget(tabs);
    tabs->setText("High Frequency",0);
    tabs->setText("Low Frequency",1);
    tabs->setText("Frequency Ratio",2);
    tabs->setText("Time Interval",3);
}

void CounterWindow::createHighFrequencyDisplay(void){
    HFDisplay = addDisplayWidgetToTab(0);
}

void CounterWindow::createLowFrequencyDisplay(void){
    LFDisplay1 = addDisplayWidgetToTab(1);
    LFDisplay2 = addDisplayWidgetToTab(1);
}

WidgetDisplay *CounterWindow::addDisplayWidgetToTab(int tabNumber){
    QString unitsStyleSheet, avgStyleSheet, errStyleSheet;

    unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    WidgetDisplay *frequencyDisplay = new WidgetDisplay("Frequency", unitsStyleSheet, true, tabs);
    avgStyleSheet = "image: url(:/graphics/graphics/units_avg.png); border: none;";
    frequencyDisplay->setAvgStyle(avgStyleSheet);
    errStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    frequencyDisplay->setErrStyle(errStyleSheet);

    frequencyDisplay->setContentsMargins(0, 6, 0, 0);
    frequencyDisplay->displayNumber("0.00");

    tabs->getLayout(tabNumber)->addWidget(frequencyDisplay);

    return frequencyDisplay;
}

WidgetDisplay *CounterWindow::hfDisplay(){
    return HFDisplay;
}

WidgetDisplay *CounterWindow::lfDisplay1(){
    return LFDisplay1;
}

WidgetDisplay *CounterWindow::lfDisplay2(){
    return LFDisplay2;
}
