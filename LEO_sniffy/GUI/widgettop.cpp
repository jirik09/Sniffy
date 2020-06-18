/*
Widget to be shown on top of the feature setting area - inntended to be used when different settings wan tto be shown.
- number of selections can be specified up to 6
- act as a radio buttons
- images or text can be added to buttons.

Comment: This could be done by tabs but I did it like this. It would be easier but shit happens.
*/
#include "widgettop.h"
#include "ui_widgettop.h"

WidgetTop::WidgetTop(QWidget *parent, int num) :
    QWidget(parent),
    ui(new Ui::WidgetTop)
{
    ui->setupUi(this);
    if(num<=1)
        ui->pushButton_2->hide();
    if(num<=2)
        ui->pushButton_3->hide();
    if(num<=3)
        ui->pushButton_4->hide();
    if(num<=4)
        ui->pushButton_5->hide();
    if(num<=5)
        ui->pushButton_6->hide();
    setSelected(0);
}

WidgetTop::~WidgetTop()
{
    delete ui;
}

void WidgetTop::setText(QString text, int index){
   switch (index){
   case 0:
       ui->pushButton_1->setText(text);
       break;
   case 1:
       ui->pushButton_2->setText(text);
       break;
   case 2:
       ui->pushButton_3->setText(text);
       break;
   case 3:
       ui->pushButton_4->setText(text);
       break;
   case 4:
       ui->pushButton_5->setText(text);
       break;
   case 5:
       ui->pushButton_6->setText(text);
       break;
   }
}

void WidgetTop::setImage(QString uri, int index){
    switch (index){
    case 0:
        ui->pushButton_1->setIcon(QIcon(uri));
        ui->pushButton_1->setText("");
        break;
    case 1:
        ui->pushButton_2->setIcon(QIcon(uri));
        ui->pushButton_2->setText("");
        break;
    case 2:
        ui->pushButton_3->setIcon(QIcon(uri));
        ui->pushButton_3->setText("");
        break;
    case 3:
        ui->pushButton_4->setIcon(QIcon(uri));
        ui->pushButton_4->setText("");
        break;
    case 4:
        ui->pushButton_5->setIcon(QIcon(uri));
        ui->pushButton_5->setText("");
        break;
    case 5:
        ui->pushButton_6->setIcon(QIcon(uri));
        ui->pushButton_6->setText("");
        break;
    }

}

void WidgetTop::setSelected (int index){
    clearSelected();
    switch (index){
    case 0:
        ui->pushButton_1->setStyleSheet(SELECTED_STYLE);
        break;
    case 1:
        ui->pushButton_2->setStyleSheet(SELECTED_STYLE);
        break;
    case 2:
        ui->pushButton_3->setStyleSheet(SELECTED_STYLE);
        break;
    case 3:
        ui->pushButton_4->setStyleSheet(SELECTED_STYLE);
        break;
    case 4:
        ui->pushButton_5->setStyleSheet(SELECTED_STYLE);
        break;
    case 5:
        ui->pushButton_6->setStyleSheet(SELECTED_STYLE);
        break;
    }
}

void WidgetTop::clearSelected (){
    ui->pushButton_1->setStyleSheet(NOT_SELECTED_STYLE);
    ui->pushButton_2->setStyleSheet(NOT_SELECTED_STYLE);
    ui->pushButton_3->setStyleSheet(NOT_SELECTED_STYLE);
    ui->pushButton_4->setStyleSheet(NOT_SELECTED_STYLE);
    ui->pushButton_5->setStyleSheet(NOT_SELECTED_STYLE);
    ui->pushButton_6->setStyleSheet(NOT_SELECTED_STYLE);
    }

QPushButton * WidgetTop::getPushButton(int index){
    switch (index){
    case 0:
        return ui->pushButton_1;
        break;
    case 1:
        return ui->pushButton_2;
        break;
    case 2:
        return ui->pushButton_3;
        break;
    case 3:
        return ui->pushButton_4;
        break;
    case 4:
        return ui->pushButton_5;
        break;
    case 5:
        return ui->pushButton_6;
        break;
    }
    return nullptr;
}
