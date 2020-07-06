#include "widgetdisplay.h"
#include "ui_widgetdisplay.h"

WidgetDisplay::WidgetDisplay(bool showPrgrssBar, QString units, QString label_1, QString label_2, QString label_3, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetDisplay)
{
    ui->setupUi(this);

    showPrgrssBar ? ui->progressBar->show() : ui->progressBar->hide();

    ui->label_unit->setText(units);  

    ui->lcdNumber->setStyleSheet("QLabel { color : white; }");
    ui->lcdNumber->display(0);

    ui->label_1->setStyleSheet("QLabel { color : white; }");
    ui->label_2->setStyleSheet("QLabel { color : white; }");
    ui->label_3->setStyleSheet("QLabel { color : white; }");
    ui->label_1->setText(label_1);
    ui->label_2->setText(label_2);
    ui->label_3->setText(label_3);
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

QString WidgetDisplay::getLabelText(int labelNumer){
    QString text;
    switch (labelNumer) {
    case 1 :
        text = ui->label_1->text();
        break;
    case 2 :
        text = ui->label_2->text();
        break;
    case 3 :
        text = ui->label_3->text();
        break;
    default:
        break;
    }
    return text;
}

void WidgetDisplay::setLabelText(int labelNumer, const QString text){
    switch (labelNumer) {
    case 1 :
        ui->label_1->setText(text);
        break;
    case 2 :
        ui->label_2->setText(text);
        break;
    case 3 :
        ui->label_3->setText(text);
        break;
    default:
        break;
    }
}

void WidgetDisplay::hideLabel(int labelNumer){
    switch (labelNumer) {
    case 1 :
        ui->label_1->hide();
        break;
    case 2 :
        ui->label_2->hide();
        break;
    case 3 :
        ui->label_3->hide();
        break;
    default:
        break;
    }
}

void WidgetDisplay::showLabel(int labelNumer){
    switch (labelNumer) {
    case 1 :
        ui->label_1->show();
        break;
    case 2 :
        ui->label_2->show();
        break;
    case 3 :
        ui->label_3->show();
        break;
    default:
        break;
    }
}

WidgetDisplay::~WidgetDisplay()
{
    delete ui;
}
