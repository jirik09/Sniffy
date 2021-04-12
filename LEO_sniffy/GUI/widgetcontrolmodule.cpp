/*
Widget to be shown on the left selection in centralWidget
- This widget controls the functions / features to be anabled. It acts as a button
- Can have icon on the left and text desription
- On the right side is shown stauts of the feature (Play, Stop, Idle, Error)
*/
#include "widgetcontrolmodule.h"
#include "ui_widgetmodule.h"

WidgetControlModule::WidgetControlModule(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetFeature)
{
    ui->setupUi(this);

    // TODO: put in global style
    QString style = "QPushButton {text-align:left;}"
                    "QPushButton:hover{background-color: rgb(38, 38, 38);text-align:left;border-radius: 5px;}";
    ui->pushButton_name->setStyleSheet(style);

    ui->pushButton_name->setText(name);
    ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_stop.png)");
    connect(ui->pushButton_name,SIGNAL(clicked()),this,SLOT(clickedInternal()));

    status = ModuleStatus::STOP;
}

WidgetControlModule::~WidgetControlModule()
{
    delete ui;
}

void WidgetControlModule::setStatus(ModuleStatus stat){
    if(stat==ModuleStatus::PLAY)
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_play.png)");

    if(stat==ModuleStatus::STOP)
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_stop.png)");

    if(stat==ModuleStatus::HIDDEN_PLAY)
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_play_hidden.png)");

    if(stat==ModuleStatus::PAUSE || stat==ModuleStatus::HIDDEN_PAUSE)
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_pause.png)");

    if(stat==ModuleStatus::LOCKED)
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_locked.png)");

    status = stat;
}

ModuleStatus WidgetControlModule::getStatus(void){
    return status;
}

void WidgetControlModule::hideStatus(){
    ui->widget_status->hide();
}

void WidgetControlModule::clickedInternal(){
    emit clicked (status);
}

void WidgetControlModule::setIcon (QString ImageURI){
    ui->pushButton_name->setIcon(QIcon(ImageURI));
}

void WidgetControlModule::setName (QString name){
    ui->pushButton_name->setText(name);
}


