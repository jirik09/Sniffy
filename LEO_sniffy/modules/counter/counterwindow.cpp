#include "counterwindow.h"
#include "ui_counterwindow.h"

CounterWindow::CounterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);

    createCounterTabs();
    createTwoDisplays();

    getDisplay2()->hide();

    tabHighFreq = new CounterTabHighFreq(tabs->getLayout(0), tabs);
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

void CounterWindow::createTwoDisplays(void){
    display1 = addDisplayWidgetToMeasArea();
    display1->setContentsMargins(5, 5, 5, 5);
    display2 = addDisplayWidgetToMeasArea();
    display2->setContentsMargins(5, 0, 5, 5);
}

WidgetDisplay *CounterWindow::addDisplayWidgetToMeasArea(void){
    QString unitsStyleSheet, avgStyleSheet, errStyleSheet;

    unitsStyleSheet = "image: url(:/graphics/graphics/units_hz.png); border: none;";
    WidgetDisplay *display = new WidgetDisplay("Frequency", unitsStyleSheet, true, this);
    avgStyleSheet = "image: url(:/graphics/graphics/units_avg.png); border: none;";
    display->setAvgStyle(avgStyleSheet);
    errStyleSheet = "image: url(:/graphics/graphics/units_err.png); border: none;";
    display->setErrStyle(errStyleSheet);

    display->displayNumber("0.00");
    ui->verticalLayout_display->addWidget(display);

    return display;
}

WidgetDisplay *CounterWindow::getDisplay1(){
    return display1;
}

WidgetDisplay *CounterWindow::getDisplay2(){
    return display2;
}

