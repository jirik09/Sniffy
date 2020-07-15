#include "widgetdisplay.h"
#include "ui_widgetdisplay.h"

WidgetDisplay::WidgetDisplay(QString firstLabelText, QString &unitsStyleSheet, bool showPrgrssBar, QWidget *parent) :
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
        label->hide();
    }

    ui->label_0->setText(firstLabelText);
    ui->label_0->show();

    setUnitsStyle(unitsStyleSheet);

    showPrgrssBar ? ui->progressBar->show() : ui->progressBar->hide();
    ui->lcdNumber->display(0.00);
}

void WidgetDisplay::setUnitsStyle(QString &unitsStyleSheet){
    ui->units->setStyleSheet(unitsStyleSheet);
}

void WidgetDisplay::setAvgStyle(QString &avgStyleSheet){
    ui->avg->setStyleSheet(avgStyleSheet);
}

void WidgetDisplay::setErrStyle(QString &errStyleSheet){
    ui->err->setStyleSheet(errStyleSheet);
}

void WidgetDisplay::displayNumber(double number){
    ui->lcdNumber->display(number);
}

void WidgetDisplay::displayNumber(const QString &string){
    ui->lcdNumber->display(string);
}

void WidgetDisplay::displayAvgNumber(double number){
    ui->lcdNumber_avg->display(number);
}

void WidgetDisplay::displayAvgNumber(const QString &string){
    ui->lcdNumber_avg->display(string);
}

void WidgetDisplay::displayErrNumber(double number){
    ui->lcdNumber_err->display(number);
}

void WidgetDisplay::displayErrNumber(const QString &string){
    ui->lcdNumber_err->display(string);
}

void WidgetDisplay::hideProgressBar(){
    ui->progressBar->hide();
}

void WidgetDisplay::showProgressBar(){
    ui->progressBar->show();
}

QString WidgetDisplay::getLabelText(int labelNumber){
    QString text = labelList.at(labelNumber)->text();
    return text;
}

void WidgetDisplay::setLabelText(int labelNumber, const QString text){
    labelList.at(labelNumber)->setText(text);
    labelList.at(labelNumber)->show();
}

void WidgetDisplay::hideLabel(int labelNumber){
    labelList.at(labelNumber)->hide();
}

void WidgetDisplay::showLabel(int labelNumber){
    labelList.at(labelNumber)->show();
}

WidgetDisplay::~WidgetDisplay()
{
    delete ui;
}
