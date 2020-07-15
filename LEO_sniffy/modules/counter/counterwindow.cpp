#include "counterwindow.h"
#include "ui_counterwindow.h"

CounterWindow::CounterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);

    createTwoDisplays();
    displayChannel2->hide();

    createCounterTabs();
    tabHighFreq = new CounterTabHighFreq(tabs->getLayout(0), tabs);
    //tabLowFreq = new CounterTabLowFreq(tabs->getLayout(1), tabs);

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

WidgetDisplay *CounterWindow::getDisplayChannel1(){
    return displayChannel1;
}

WidgetDisplay *CounterWindow::getDisplayChannel2(){
    return displayChannel2;
}

widgetTab *CounterWindow::getTabs(void){
    return tabs;
}



