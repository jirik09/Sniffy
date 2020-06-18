/*
Widget to be shown on the left selection in centralWidget
- This widget controls the functions / features to be anabled. It acts as a button
- Can have icon on the left and text desription
- On the right side is shown stauts of the feature (Play, Stop, Idle, Error)
*/
#include "widgetmodule.h"
#include "ui_widgetfeature.h"

WidgetModule::WidgetModule(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetFeature)
{
    ui->setupUi(this);

        ui->pushButton_name->setText(name);
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_stop.png)"));
        connect(ui->pushButton_name,SIGNAL(clicked()),this,SLOT(clickedInternal()));

        status = ModuleStatus::STOP;
}

WidgetModule::~WidgetModule()
{
    delete ui;
}

void WidgetModule::setStatus(ModuleStatus stat){
    if(stat==ModuleStatus::PLAY)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_play.png)"));

    if(stat==ModuleStatus::STOP)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_stop.png)"));

    if(stat==ModuleStatus::PAUSE)
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_pause.png)"));

    status = stat;
}

ModuleStatus WidgetModule::getStatus(void){
    return status;
}

void WidgetModule::clickedInternal(){
    emit clicked (status);
}

void WidgetModule::setIcon (QString ImageURI){
    ui->pushButton_name->setIcon(QIcon(ImageURI));
}

