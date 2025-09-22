/*
Separator for the widgets with grey label and grey line
*/
#include "widgetseparator.h"
#include "ui_widgetseparator.h"
#include "stylehelper.h"

WidgetSeparator::WidgetSeparator(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetSeparator)
{
    ui->setupUi(this);
    ui->widget->setStyleSheet(StyleHelper::backgroundImage(Graphics::getGraphicsPath()+"separator.png"));
    ui->label_name->setStyleSheet(StyleHelper::separatorLabel(Graphics::COLOR_TEXT_LABEL));
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
