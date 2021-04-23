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
    ui->pushButton_name->setText(name);

    ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getGraphicsPath()+"status_stop.png)");
    connect(ui->pushButton_name,SIGNAL(clicked()),this,SLOT(clickedInternal()));

    status = ModuleStatus::STOP;
}

WidgetControlModule::~WidgetControlModule()
{
    delete ui;
}

void WidgetControlModule::setStatus(ModuleStatus stat){
    if(stat==ModuleStatus::PLAY){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getGraphicsPath()+"status_play.png)");
    }
    if(stat==ModuleStatus::STOP){
        ui->pushButton_name->setIcon(iconAvail);
        ui->pushButton_name->setEnabled(true);
        ui->pushButton_name->setChecked(false);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getGraphicsPath()+"status_stop.png)");        
    }
    if(stat==ModuleStatus::HIDDEN_PLAY){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getGraphicsPath()+"status_play_hidden.png)");        
    }
    if(stat==ModuleStatus::PAUSE || stat==ModuleStatus::HIDDEN_PAUSE){
        ui->pushButton_name->setChecked(true);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getGraphicsPath()+"status_pause.png)");        
    }
    if(stat==ModuleStatus::LOCKED){
        ui->pushButton_name->setIcon(iconLock);
        ui->pushButton_name->setEnabled(false);
        ui->widget_status->setStyleSheet("margin: 2px;image: url("+Graphics::getGraphicsPath()+"status_locked.png)");                
    }
    status = stat;
}

ModuleStatus WidgetControlModule::getStatus(void){
    return status;
}

void WidgetControlModule::hideStatus(){
    ui->widget_status->hide();
}

void WidgetControlModule::mousePressEvent(QMouseEvent *event){
    emit mouseWheelPressEvent(event);
}

void WidgetControlModule::clickedInternal(){
    emit clicked(status);
}

void WidgetControlModule::setIcon (QString ImageURI){
    QPixmap icon(ImageURI);

    QPixmap iconAreaAvail(icon.size());
    QPixmap iconAreaLock(icon.size());

    iconAreaAvail.fill(Graphics::COLOR_TEXT_ALL);
    iconAreaLock.fill(Graphics::COLOR_COMPONENT_DISABLED);

    iconAreaAvail.setMask(icon.createMaskFromColor(Qt::transparent));
    iconAreaLock.setMask(icon.createMaskFromColor(Qt::transparent));

    this->iconAvail.addPixmap(iconAreaAvail,QIcon::Normal);
    this->iconLock.addPixmap(iconAreaLock,QIcon::Disabled);

    ui->pushButton_name->setIcon(iconAvail);
}

void WidgetControlModule::setName (QString name){
    ui->pushButton_name->setText(name);
}


