/*
Widget to be shown on very bottom of the centralWidget
*/
#include "widgetfooter.h"
#include "ui_widgetfooter.h"

WidgetFooter::WidgetFooter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFooter)
{
    ui->setupUi(this);

    //connect(ui->pushButton_setSize,&QPushButton::clicked,this, &WidgetFooter::sizeCallback);
    connect(ui->label_burgerAnim,&ClickableLabel::clicked,this, &WidgetFooter::sizeCallback);
    connect(ui->pushButton_settingsA,&QPushButton::clicked,this, &WidgetFooter::settingsCallback);
    connect(ui->pushButton_settingsB,&QPushButton::clicked,this, &WidgetFooter::settingsCallback);

    ui->pushButton_settingsA->setIcon(Graphics::tintedIcon(Graphics::getCommonPath()+"settings_gear.png"));
    ui->pushButton_settingsB->setIcon(Graphics::tintedIcon(Graphics::getCommonPath()+"settings_gear.png"));
    ui->pushButton_settingsA->setStyleSheet("QPushButton{border:none;} QPushButton:hover{border:none;background-color:"+Graphics::palette().windowControlHover+";}");
    ui->pushButton_settingsB->setStyleSheet("QPushButton{border:none;} QPushButton:hover{border:none;background-color:"+Graphics::palette().windowControlHover+";}");

    ui->label_burgerAnim->setStyleSheet("QLabel:hover{background-color:"+Graphics::palette().windowControlHover+";}");
    ui->label_burgerAnim->setAlignment(Qt::AlignCenter);

    setAppearance(isWide);
}

WidgetFooter::~WidgetFooter()
{
    delete ui;
}

void WidgetFooter::settingsCallback(){
    emit settingsClicked();

}
void WidgetFooter::sizeCallback(){
    if(isWide){
        isWide=false;
    }else{
        isWide=true;
    }
    setAppearance(isWide);
    emit sizeClicked(isWide);
}

void WidgetFooter::setAppearance(bool wide){
    this->isWide = wide; //save the value if it is called from public
    if(!wide){
        ui->pushButton_settingsA->show();
        ui->pushButton_settingsB->hide();
        ui->label_settings->setText("");
        //ui->pushButton_setSize->setIcon(QIcon(Graphics::getGraphicsPath()+"settings_horizontal.png"));

        ui->label_burgerAnim->setPixmap(Graphics::tintedPixmap(Graphics::getCommonPath()+"menu_burger_button_backw.png"));
    }else{
        ui->pushButton_settingsB->show();
        ui->pushButton_settingsA->hide();
        ui->label_settings->setText("Settings");
        //ui->pushButton_setSize->setIcon(QIcon(Graphics::getGraphicsPath()+"settings_vertical.png"));

        ui->label_burgerAnim->setPixmap(Graphics::tintedPixmap(Graphics::getCommonPath()+"menu_burger_button.png"));
    }
}

