/*
Widget to be shown on the left selection in centralWidget
- This widget controls the functions / features to be anabled. It acts as a button
- Can have icon on the left and text desription
- On the right side is shown stauts of the feature (Play, Stop, Idle, Error)
*/
#include "widgetfeature.h"
#include "ui_widgetfeature.h"

WidgetFeature::WidgetFeature(QWidget *parent, FeatureIcon icon, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetFeature)
{
    ui->setupUi(this);
    if(icon == FeatureIcon::SCOPE)
        ui->pushButton_name->setIcon(QIcon(":/graphics/graphics/icon_scope.png"));
        ui->pushButton_name->setText(name);
        ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_stop.png)"));
}

WidgetFeature::~WidgetFeature()
{
    delete ui;
}

void WidgetFeature::setStatus(FeatureStatus status){
    if(status==FeatureStatus::PLAY)
    ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_play.png)"));

    if(status==FeatureStatus::STOP)
    ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_stop.png)"));

    if(status==FeatureStatus::PAUSE)
    ui->widget_status->setStyleSheet(QString::fromUtf8("image: url(:/graphics/graphics/status_pause.png)"));
}

QPushButton * WidgetFeature::getPushButton(){
    return ui->pushButton_name;
}
