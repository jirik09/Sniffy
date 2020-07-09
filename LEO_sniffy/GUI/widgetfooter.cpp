/*
Widget to be shown on very bottom of the centralWidget
*/
#include "widgetfooter.h"
#include "ui_widgetfooter.h"

WidgetFooter::WidgetFooter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFooter)
{
    ui->setupUi(this);

    connect(ui->pushButton_setSize,&QPushButton::clicked,this, &WidgetFooter::sizeCallback);
    connect(ui->pushButton_settingsA,&QPushButton::clicked,this, &WidgetFooter::settingsCallback);
    connect(ui->pushButton_settingsB,&QPushButton::clicked,this, &WidgetFooter::settingsCallback);

    ui->pushButton_settingsA->setIcon(QIcon(":/graphics/graphics/settings_gear.png"));
    ui->pushButton_settingsB->setIcon(QIcon(":/graphics/graphics/settings_gear.png"));

    setAppearance(isWide);
}

WidgetFooter::~WidgetFooter()
{
    delete ui;
}

void WidgetFooter::settingsCallback(){
    emit settingsClicked();

}
void WidgetFooter::sizeCallback(){
    if(isWide){
        isWide=false;
    }else{
        isWide=true;
    }
    setAppearance(isWide);
    emit sizeClicked(isWide);
}

void WidgetFooter::setAppearance(bool isWide){
    if(!isWide){
        ui->pushButton_settingsA->show();
        ui->pushButton_settingsB->hide();
        ui->label_settings->setText("");
        ui->pushButton_setSize->setIcon(QIcon(":/graphics/graphics/settings_horizontal.png"));
    }else{
        ui->pushButton_settingsB->show();
        ui->pushButton_settingsA->hide();
        ui->label_settings->setText("Settings");
        ui->pushButton_setSize->setIcon(QIcon(":/graphics/graphics/settings_vertical.png"));
    }
}

