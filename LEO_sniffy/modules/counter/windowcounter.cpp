/*
lol
*/

#include "windowcounter.h"
#include "ui_windowcounter.h"

WindowCounter::WindowCounter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindowCounter)
{
    ui->setupUi(this);

    //********************* insert top options *********************
    //widgetTab *tabs = new widgetTab(ui->widget_display,4);
//    ui->verticalLayout_settings->addWidget(tabs);
//    tabs->setText("High Frequency",0);
//    tabs->setText("Low Frequency",1);
//    tabs->setText("Frequency Ratio",2);
//    tabs->setText("Time Interval",3);
}


WindowCounter::~WindowCounter()
{
    delete ui;
}
