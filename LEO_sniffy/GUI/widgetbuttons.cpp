/*
Class for buttons.
- Number of buttons up to 8 can be specified.
- Buttons can be Normal type, Checkable or Radio
- Each button can have its own color and text
- Possible to show text label on the left

*/
#include "widgetbuttons.h"
#include "ui_widgetbuttons.h"

WidgetButtons::WidgetButtons(QWidget *parent, int num,ButtonTypes type, QString name, int defaultSelectedIndex, int optionalEmitParam) :
    QWidget(parent),
    ui(new Ui::WidgetButtons),
    optionalEmitParam(optionalEmitParam)
{
    ui->setupUi(this);
    setObjectName(name);

    this->type = type;
    if(name != ""){
        ui->label->setText(name);
    }else{
        ui->label->hide();
    }

    pushButtonsList.append(ui->pushButton_1);
    pushButtonsList.append(ui->pushButton_2);
    pushButtonsList.append(ui->pushButton_3);
    pushButtonsList.append(ui->pushButton_4);
    pushButtonsList.append(ui->pushButton_5);
    pushButtonsList.append(ui->pushButton_6);
    pushButtonsList.append(ui->pushButton_7);
    pushButtonsList.append(ui->pushButton_8);


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
        foreach(QPushButton *btn, pushButtonsList){
            btn->setCheckable(true);
        }
        setChecked(true,defaultSelectedIndex);
    }

    for(int i = 0; i < 8 ; i++){
        setColor(Graphics::COLOR_CONTROLS,i);
    }

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

QByteArray WidgetButtons::saveGeometry()
{
    return  QByteArray::number(getStatus());
}

void WidgetButtons::restoreGeometry(QByteArray geom)
{
    uncheckAll();
    if(type == ButtonTypes::RADIO){
        int index = 0;
        if(geom.toInt()>2){
            index = (int)(qLn(geom.toInt())/qLn(2)+0.01);
        }else{
            index = geom.toInt()-1;
        }
        setChecked(true,index);
    }else{
        int status = geom.toInt();
        if(status & 0x01){
            setChecked(true,0);
        }
        if(status & 0x02){
            setChecked(true,1);
        }
        if(status & 0x04){
            setChecked(true,2);
        }
        if(status & 0x08){
            setChecked(true,3);
        }
        if(status & 0x10){
            setChecked(true,4);
        }
        if(status & 0x20){
            setChecked(true,5);
        }
        if(status & 0x40){
            setChecked(true,6);
        }
        if(status & 0x80){
            setChecked(true,7);
        }
    }
}

void WidgetButtons::setText(QString text, int index){
    pushButtonsList.at(index)->setText(text);
}

QString WidgetButtons::getText(int index){
    return pushButtonsList.at(index)->text();
}

void WidgetButtons::setColor(QString text, int index){
    QString tempStyleSheet = "";
    if (index>=0 && index<8){
        if(type == ButtonTypes::CHECKABLE || type == ButtonTypes::RADIO){            
            tempStyleSheet = Graphics::STYLE_CHECK_BUTTON+"QPushButton:checked{background-color:"+text+";}";
            //tempStyleSheet = QString(Graphics::STYLE_CHECK_BUTTON).arg(text);
        }else{            
            tempStyleSheet = Graphics::STYLE_PUSH_BUTTON+"QPushButton{background-color:"+text+";}";
            //tempStyleSheet = QString(Graphics::STYLE_PUSH_BUTTON).arg(text);
        }
        pushButtonsList.at(index)->setStyleSheet(tempStyleSheet);
    }
}

void WidgetButtons::clickedInternal (int index){
    if(type==ButtonTypes::RADIO){
        uncheckAll();
        setChecked(true, index);
    }
    emit clicked(index,optionalEmitParam);

    int status = 0;
    for (int i=0;i<8;i++) {
        if(isChecked(i)){
            status += qPow(2,i);
        }
    }
    emit statusChanged(status, optionalEmitParam);
}

bool WidgetButtons::isChecked (int index){
    return pushButtonsList.at(index)->isChecked();
}

int WidgetButtons::getSelectedIndex(){
    foreach(QPushButton *pb, pushButtonsList){
        if(pb->isChecked())
            return pushButtonsList.indexOf(pb);
    }
    return -1;
}

int WidgetButtons::getStatus(){
    int status = 0;

    for (int i=0;i<8;i++) {
        if(isChecked(i)){
            status += qPow(2,i);
        }
    }
    return status;
}

void WidgetButtons::setDisabledButton(bool disabled, int index){
    pushButtonsList.at(index)->setDisabled(disabled);
}
void WidgetButtons::disableAll(){
    for (int i=0;i<8;i++) {
        setDisabledButton(true, i);
    }
}

void WidgetButtons::setEnabledButton(bool enable, int index){
    pushButtonsList.at(index)->setEnabled(enable);
}
void WidgetButtons::enableAll(bool enable){
    for (int i=0;i<8;i++) {
        setEnabledButton(enable, i);
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
    foreach(QPushButton *btn, pushButtonsList){
        btn->setChecked(false);
    }
}

void WidgetButtons::setChecked (bool checked, int index){
    if(type==ButtonTypes::RADIO){
        uncheckAll();
    }
    pushButtonsList.at(index)->setChecked(checked);
}

void WidgetButtons::setButtonHidden(bool hidden, int index)
{
    int i = 0;
    foreach(QPushButton *btn, pushButtonsList){
        if(index==i){
            if(hidden){
                btn->hide();
            }else{
                btn->show();
            }
        }
        i++;
    }
}
