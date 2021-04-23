/*
Widget with label. nothing special.
*/
#include "widgetlabel.h"
#include "ui_widgetlabel.h"

WidgetLabel::WidgetLabel(QWidget *parent, QString name, QString value) :
    QWidget(parent),
    ui(new Ui::WidgetLabel)
{
    ui->setupUi(this);
    ui->label_name->setText(name);
    ui->label_value->setText(value);
}

WidgetLabel::~WidgetLabel()
{
    delete ui;
}

void WidgetLabel::setName(QString name){
    ui->label_name->setText(name);
}
void WidgetLabel::setValue(QString value){
    ui->label_value->setText(value);
}

void WidgetLabel::setColor(QString color)
{
    QString style = "QLabel{color:"+color+";}";
    ui->label_name->setStyleSheet(style);
    ui->label_value->setStyleSheet(style);
}
