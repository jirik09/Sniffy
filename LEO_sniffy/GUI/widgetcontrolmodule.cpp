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

        ui->pushButton_name->setText(name);
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_stop.png)"));
        connect(ui->pushButton_name,SIGNAL(clicked()),this,SLOT(clickedInternal()));

        status = ModuleStatus::STOP;
}

WidgetControlModule::~WidgetControlModule()
{
    delete ui;
}

void WidgetControlModule::setStatus(ModuleStatus stat){
    if(stat==ModuleStatus::PLAY)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_play.png)"));

    if(stat==ModuleStatus::STOP)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_stop.png)"));

    if(stat==ModuleStatus::HIDDEN_PLAY)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_play_hidden.png)"));


    if(stat==ModuleStatus::PAUSE || stat==ModuleStatus::HIDDEN_PAUSE)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_pause.png)"));

    status = stat;
}

ModuleStatus WidgetControlModule::getStatus(void){
    return status;
}

void WidgetControlModule::clickedInternal(){
    emit clicked (status);
}

void WidgetControlModule::setIcon (QString ImageURI){
    ui->pushButton_name->setIcon(QIcon(ImageURI));
}
