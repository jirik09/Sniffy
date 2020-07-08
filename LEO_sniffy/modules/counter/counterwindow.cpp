#include "counterwindow.h"
#include "ui_windowcounter.h"

CounterWindow::CounterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);

    createDisplayTabs();
    createHighFrequencyTab();
    createLowFrequencyTab();
}

void CounterWindow::createDisplayTabs(void){
    tabs = new widgetTab(ui->widget_display,4);
    ui->verticalLayout_display->addWidget(tabs);
    tabs->setText("High Frequency",0);
    tabs->setText("Low Frequency",1);
    tabs->setText("Frequency Ratio",2);
    tabs->setText("Time Interval",3);
}

void CounterWindow::createHighFrequencyTab(void){
    addDisplayWidgetToTab(0);
}

void CounterWindow::createLowFrequencyTab(void){
    addDisplayWidgetToTab(1);
    addDisplayWidgetToTab(1);
}

void CounterWindow::addDisplayWidgetToTab(int tabNumber){
    QString unitsStyleSheet, avgStyleSheet, errStyleSheet;

    unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    WidgetDisplay *frequencyDisplay = new WidgetDisplay("Frequency", unitsStyleSheet, true, tabs);

    avgStyleSheet = "image: url(:/graphics/graphics/units_avg.png); border: none;";
    frequencyDisplay->setAvgStyle(avgStyleSheet);
    errStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    frequencyDisplay->setErrStyle(errStyleSheet);
    frequencyDisplay->setContentsMargins(0, 6, 0, 0);
    frequencyDisplay->displayNumber("255 256.0531");

    tabs->getLayout(tabNumber)->addWidget(frequencyDisplay);
}

CounterWindow::~CounterWindow()
{
    delete ui;
}
