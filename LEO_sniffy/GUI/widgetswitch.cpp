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
    setObjectName(name);

    ui->pushButton_left->setStyleSheet(styleSelected);
    ui->pushButton_right->setStyleSheet(styleNotSelected);

    ui->pushButton_left->setText(leftName);
    ui->pushButton_right->setText(rightName);

    connect(ui->pushButton_left,SIGNAL(clicked()),this,SLOT(setLeft()));
    connect(ui->pushButton_right,SIGNAL(clicked()),this,SLOT(setRight()));
}

WidgetSwitch::~WidgetSwitch()
{
    delete ui;
}

QByteArray WidgetSwitch::saveGeometry()
{
    if(isCheckedLeft()){
        return "L";
    }else{
        return "R";
    }
}

void WidgetSwitch::restoreGeometry(QByteArray geom)
{
    if (geom == "L"){
        setLeft(true);
    }else{
        setRight(true);
    }
}

void WidgetSwitch::setLeft(bool silent){
    ui->pushButton_left->setStyleSheet(styleSelected);
    ui->pushButton_right->setStyleSheet(styleNotSelected);
    if (!silent)
        emit clicked(0);
}

void WidgetSwitch::setRight(bool silent){
    ui->pushButton_right->setStyleSheet(styleSelected);
    ui->pushButton_left->setStyleSheet(styleNotSelected);
    if(!silent)
        emit clicked(1);
}

bool WidgetSwitch::isCheckedLeft(){
    return (ui->pushButton_left->isChecked());
}

void WidgetSwitch::setColor(QString color){
    styleSelected = color;
    if(ui->pushButton_right->isChecked()){
        ui->pushButton_right->setStyleSheet(styleSelected);
        ui->pushButton_left->setStyleSheet(styleNotSelected);
    }else {
        ui->pushButton_left->setStyleSheet(styleSelected);
        ui->pushButton_right->setStyleSheet(styleNotSelected);
    }
}

void WidgetSwitch::enable(){
    setLeft(true);
}

bool WidgetSwitch::isCheckedRight(){
    return (ui->pushButton_right->isChecked());
}



