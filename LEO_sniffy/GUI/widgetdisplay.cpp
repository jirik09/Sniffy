#include "widgetdisplay.h"
#include "ui_widgetdisplay.h"

WidgetDisplay::WidgetDisplay(QString firstLabelText, QString units, bool showPrgrssBar, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetDisplay)
{
    ui->setupUi(this);

    labelList.append(ui->label_0);
    labelList.append(ui->label_1);
    labelList.append(ui->label_2);
    labelList.append(ui->label_3);
    labelList.append(ui->label_4);

    foreach(QLabel* label, labelList){
        label->setStyleSheet("QLabel { color : white; }");
        label->hide();
    }

    ui->label_0->setText(firstLabelText);
    ui->label_0->show();

    ui->label_unit->setText(units);
    showPrgrssBar ? ui->progressBar->show() : ui->progressBar->hide();

    ui->lcdNumber->setStyleSheet("QLabel { color : white; }");
    ui->lcdNumber->display(0);
}

void WidgetDisplay::setUnits(QString units){
    ui->label_unit->setText(units);
}

void WidgetDisplay::displayNumber(double number){
    ui->lcdNumber->display(number);
}

void WidgetDisplay::hideProgressBar(){
    ui->progressBar->hide();
}

void WidgetDisplay::showProgressBar(){
    ui->progressBar->show();
}

QString WidgetDisplay::getLabelText(int labelNumber){
    QString text = labelList[labelNumber]->text();
    return text;
}

void WidgetDisplay::setLabelText(int labelNumber, const QString text){
    labelList[labelNumber]->setText(text);
    labelList[labelNumber]->show();
}

void WidgetDisplay::hideLabel(int labelNumber){
    labelList[labelNumber]->hide();
}

void WidgetDisplay::showLabel(int labelNumber){
    labelList[labelNumber]->show();
}

WidgetDisplay::~WidgetDisplay()
{
    delete ui;
}
