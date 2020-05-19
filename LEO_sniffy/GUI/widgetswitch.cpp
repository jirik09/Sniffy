/*
Widget for On/Off switch. Label with name on the left

Comment: similar to button widget set to 2 buttons, radio and on off text. Difference is distance between buttons
*/
#include "widgetswitch.h"
#include "ui_widgetswitch.h"

WidgetSwitch::WidgetSwitch(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetSwitch)
{
    ui->setupUi(this);
    ui->label_name->setText(name);
    setOff();
    connect(ui->pushButton_on,SIGNAL(clicked()),this,SLOT(setOn()));
    connect(ui->pushButton_off,SIGNAL(clicked()),this,SLOT(setOff()));

}

WidgetSwitch::~WidgetSwitch()
{
    delete ui;
}



void WidgetSwitch::setOn(){
    ui->pushButton_on->setStyleSheet(SELECTED_STYLE);
    ui->pushButton_off->setStyleSheet(NOT_SELECTED_STYLE);
}

void WidgetSwitch::setOff(){
    ui->pushButton_on->setStyleSheet(NOT_SELECTED_STYLE);
    ui->pushButton_off->setStyleSheet(SELECTED_STYLE);
}


