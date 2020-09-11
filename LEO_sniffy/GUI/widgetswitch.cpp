/*
Widget for On/Off switch. Label with name on the left

Comment: similar to button widget set to 2 buttons, radio and on off text. Difference is distance between buttons
*/
#include "widgetswitch.h"
#include "ui_widgetswitch.h"

WidgetSwitch::WidgetSwitch(QWidget *parent, QString leftName, QString rightName, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetSwitch)
{
    ui->setupUi(this);

    ui->label_name->setText(name);

    ui->pushButton_left->setStyleSheet(SELECTED_STYLE);
    ui->pushButton_right->setStyleSheet(NOT_SELECTED_STYLE);

    ui->pushButton_left->setText(leftName);
    ui->pushButton_right->setText(rightName);

    connect(ui->pushButton_left,SIGNAL(clicked()),this,SLOT(setLeft()));
    connect(ui->pushButton_right,SIGNAL(clicked()),this,SLOT(setRight()));
}

WidgetSwitch::~WidgetSwitch()
{
    delete ui;
}

void WidgetSwitch::setLeft(){
    ui->pushButton_left->setStyleSheet(SELECTED_STYLE);
    ui->pushButton_right->setStyleSheet(NOT_SELECTED_STYLE);
    emit clicked(0);
}

void WidgetSwitch::setRight(){
    ui->pushButton_right->setStyleSheet(SELECTED_STYLE);
    ui->pushButton_left->setStyleSheet(NOT_SELECTED_STYLE);
    emit clicked(1);
}

bool WidgetSwitch::isCheckedLeft(){
    return (ui->pushButton_left->isChecked());
}

bool WidgetSwitch::isCheckedRight(){
    return (ui->pushButton_right->isChecked());
}



