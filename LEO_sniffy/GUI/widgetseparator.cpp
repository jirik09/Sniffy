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

WidgetSeparator::~WidgetSeparator()
{
    delete ui;
}
