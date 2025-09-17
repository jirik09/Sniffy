#include "widgetlogininfo.h"
#include "ui_widgetlogininfo.h"

WidgetLoginInfo::WidgetLoginInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLoginInfo)
{
    ui->setupUi(this);
    ui->pushButton->setIcon(QIcon(Graphics::getGraphicsPath()+"user.png"));

    connect(ui->pushButton,&QPushButton::clicked,this, &WidgetLoginInfo::loginCallback);
    connect(ui->label_email,&ClickableLabel::clicked,this, &WidgetLoginInfo::loginCallback);
    connect(ui->label_valid,&ClickableLabel::clicked,this, &WidgetLoginInfo::loginCallback);
    updateInfo();

}

WidgetLoginInfo::~WidgetLoginInfo()
{
    delete ui;
}

void WidgetLoginInfo::loginCallback(){
    emit loginDialogClicked();
}

void WidgetLoginInfo::updateInfo(){
    ui->label_email->setText(CustomSettings::getUserEmail());

    if(QDateTime::currentDateTime()>CustomSettings::getTokenValidity()){
        ui->pushButton->setIcon(QIcon(Graphics::getGraphicsPath()+"user_error.png"));
        ui->label_valid->setText("Please login");
    }else{
        ui->pushButton->setIcon(QIcon(Graphics::getGraphicsPath()+"user_ok.png"));
        ui->label_valid->setText("Valid till: "+CustomSettings::getTokenValidity().date().toString("dd.MM.yyyy"));
    }
}

void WidgetLoginInfo::setCompact(bool compact){
    ui->label_email->setVisible(!compact);
    ui->label_valid->setVisible(!compact);
}
