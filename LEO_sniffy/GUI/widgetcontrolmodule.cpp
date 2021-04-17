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
    ui->pushButton_name->setStyleSheet(Graphics::STYLE_MODULE_BUTTON);
    ui->pushButton_name->setCheckable(true);

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
    if(stat==ModuleStatus::PLAY){
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_play.png)");
        ui->pushButton_name->setChecked(true);
    }
    if(stat==ModuleStatus::STOP){
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_stop.png)");
        ui->pushButton_name->setEnabled(true);
        ui->pushButton_name->setChecked(false);
    }
    if(stat==ModuleStatus::HIDDEN_PLAY){
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_play_hidden.png)");
        ui->pushButton_name->setChecked(true);
    }
    if(stat==ModuleStatus::PAUSE || stat==ModuleStatus::HIDDEN_PAUSE){
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_pause.png)");
        ui->pushButton_name->setChecked(true);
    }
    if(stat==ModuleStatus::LOCKED){
        ui->widget_status->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"status_locked.png)");
        ui->pushButton_name->setEnabled(false);
    }
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


