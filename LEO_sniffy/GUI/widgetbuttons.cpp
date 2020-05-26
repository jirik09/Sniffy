/*
Class for buttons.
- Number of buttons up to 8 can be specified.
- Buttons can be Normal type, Checkable or Radio
- Each button can have its own color and text
- Possible to show text label on the left

*/
#include "widgetbuttons.h"
#include "ui_widgetbuttons.h"

WidgetButtons::WidgetButtons(QWidget *parent, int num,ButtonTypes type, QString name, int defaultSelectedIndex) :
    QWidget(parent),
    ui(new Ui::WidgetButtons)
{
    ui->setupUi(this);

    this->type = type;
    if(name != ""){
        ui->label->setText(name);
    }else{
        ui->label->hide();
    }

    pushButtonsList = new QList<QPushButton*>();

    pushButtonsList->append(ui->pushButton_1);
    pushButtonsList->append(ui->pushButton_2);
    pushButtonsList->append(ui->pushButton_3);
    pushButtonsList->append(ui->pushButton_4);
    pushButtonsList->append(ui->pushButton_5);
    pushButtonsList->append(ui->pushButton_6);
    pushButtonsList->append(ui->pushButton_7);
    pushButtonsList->append(ui->pushButton_8);


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
    if(num<=6)
        ui->pushButton_7->hide();
    if(num<=7)
        ui->pushButton_8->hide();

    if(type==ButtonTypes::CHECKABLE || type==ButtonTypes::RADIO){
        ui->pushButton_1->setCheckable(true);
        ui->pushButton_2->setCheckable(true);
        ui->pushButton_3->setCheckable(true);
        ui->pushButton_4->setCheckable(true);
        ui->pushButton_5->setCheckable(true);
        ui->pushButton_6->setCheckable(true);
        ui->pushButton_7->setCheckable(true);
        ui->pushButton_8->setCheckable(true);
        setChecked(true,defaultSelectedIndex);
    }


    setColor(BACKGROUND_COLOR_BLUE,0);
    setColor(BACKGROUND_COLOR_BLUE,1);
    setColor(BACKGROUND_COLOR_BLUE,2);
    setColor(BACKGROUND_COLOR_BLUE,3);
    setColor(BACKGROUND_COLOR_BLUE,4);
    setColor(BACKGROUND_COLOR_BLUE,5);
    setColor(BACKGROUND_COLOR_BLUE,6);
    setColor(BACKGROUND_COLOR_BLUE,7);



    connect(ui->pushButton_1,SIGNAL(clicked()),this,SLOT(button_1_Clicked()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(button_2_Clicked()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(button_3_Clicked()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(button_4_Clicked()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(button_5_Clicked()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(button_6_Clicked()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(button_7_Clicked()));
    connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(button_8_Clicked()));
}

WidgetButtons::~WidgetButtons()
{
    delete ui;
}

void WidgetButtons::setText(QString text, int index){
    pushButtonsList->at(index)->setText(text);
}

QString WidgetButtons::getText(int index){
    return pushButtonsList->at(index)->text();

}

void WidgetButtons::setColor(QString text, int index){
    QString tempStyleSheet = "";
    if (index>=0 && index<8){
        if(type == ButtonTypes::CHECKABLE || type == ButtonTypes::RADIO){
            tempStyleSheet += "QPushButton:disabled{background-color: rgb(48,48,48);color: rgb(128,128,128);} QPushButton{border: none;"+QString::fromUtf8(NOT_CHECKED_BACKGROUND)+"} QPushButton:checked{border: none;"+ text +"}";
        }else{
            tempStyleSheet += "QPushButton:disabled{background-color: rgb(48,48,48);color: rgb(128,128,128);} QPushButton:pressed{border: 2px solid rgb(48,48,48)} QPushButton{border: none;"+text +"}";
        }

        pushButtonsList->at(index)->setStyleSheet(tempStyleSheet);
    }
}

void WidgetButtons::clickedInternal (int index){
    if(type==ButtonTypes::RADIO){
        uncheckAll();
        setChecked(true, index);
    }
    emit clicked(index);
}

bool WidgetButtons::isChecked (int index){
    return pushButtonsList->at(index)->isChecked();
}

void WidgetButtons::setDisabledButton(bool state, int index){
    pushButtonsList->at(index)->setDisabled(state);
}
void WidgetButtons::disableAll(){
    for (int i=0;i<8;i++) {
        setDisabledButton(true, i);
    }
}

void WidgetButtons::button_1_Clicked(){
    clickedInternal(0);
}
void WidgetButtons::button_2_Clicked(){
    clickedInternal(1);
}
void WidgetButtons::button_3_Clicked(){
    clickedInternal(2);
}
void WidgetButtons::button_4_Clicked(){
    clickedInternal(3);
}
void WidgetButtons::button_5_Clicked(){
    clickedInternal(4);
}
void WidgetButtons::button_6_Clicked(){
    clickedInternal(5);
}
void WidgetButtons::button_7_Clicked(){
    clickedInternal(6);
}
void WidgetButtons::button_8_Clicked(){
    clickedInternal(7);
}

void WidgetButtons::uncheckAll(){
    ui->pushButton_1->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_5->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_7->setChecked(false);
    ui->pushButton_8->setChecked(false);
}

void WidgetButtons::setChecked (bool checked, int index){
    pushButtonsList->at(index)->setChecked(checked);
}
