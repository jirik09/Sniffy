/*
Widget for On/Off switch. Label with name on the left

Comment: similar to button widget set to 2 buttons, radio and on off text. Difference is distance between buttons
*/
#include "widgetswitch.h"
#include "ui_widgetswitch.h"
#include "stylehelper.h"

WidgetSwitch::WidgetSwitch(QWidget *parent, QString leftLabel, QString rightLabel, QString switchName, int optionalEmitParam) :
    QWidget(parent),
    ui(new Ui::WidgetSwitch),
    optionalEmitParam(optionalEmitParam)
{
    ui->setupUi(this);

    if(switchName != ""){
        ui->label_name->setText(switchName);
    }else{
        ui->label_name->hide();
    }
    setObjectName(switchName);

    ui->pushButton_left->setCheckable(true);
    ui->pushButton_right->setCheckable(true);
    setLeft(true);

    ui->pushButton_left->setText(leftLabel);
    ui->pushButton_right->setText(rightLabel);

    setColor(Graphics::getChannelColor(0));

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
    ui->pushButton_left->setChecked(true);
    ui->pushButton_right->setChecked(false);
    if (!silent)
        emit clicked(0,optionalEmitParam);
}

void WidgetSwitch::setRight(bool silent){
    ui->pushButton_right->setStyleSheet(styleSelected);
    ui->pushButton_left->setStyleSheet(styleNotSelected);
    ui->pushButton_right->setChecked(true);
    ui->pushButton_left->setChecked(false);
    if(!silent)
        emit clicked(1,optionalEmitParam);
}

bool WidgetSwitch::isCheckedLeft(){
    return (ui->pushButton_left->isChecked());
}

bool WidgetSwitch::isCheckedRight(){
    return (ui->pushButton_right->isChecked());
}

void WidgetSwitch::setColor(QString color)
{
    if(Graphics::STYLE_TRANSPARENCY_USED)
        color = color.remove("#");

    styleSelected = StyleHelper::switchSelected(color);
    styleNotSelected = StyleHelper::switchNotSelected(styleSelected);

    if(ui->pushButton_right->isChecked()){
        setRight(true);
    }else {
        setLeft(true);
    }
}






