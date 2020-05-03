/*
Widget with label. nothing special.
*/
#include "widgetlabel.h"
#include "ui_widgetlabel.h"

WidgetLabel::WidgetLabel(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetLabel)
{
    ui->setupUi(this);
    ui->label_mane->setText(name);
}

WidgetLabel::~WidgetLabel()
{
    delete ui;
}
