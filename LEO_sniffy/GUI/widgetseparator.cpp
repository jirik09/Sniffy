/*
Separator for the widgets with grey label and grey line
*/
#include "widgetseparator.h"
#include "ui_widgetseparator.h"

WidgetSeparator::WidgetSeparator(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetSeparator)
{
    ui->setupUi(this);
    ui->label_name->setText(name);           
}

void WidgetSeparator::hideLine(){
    ui->widget->hide();
}

void WidgetSeparator::setText(QString text){
    ui->label_name->setText(text);
}

WidgetSeparator::~WidgetSeparator()
{
    delete ui;
}
