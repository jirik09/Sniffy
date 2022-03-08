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

    ui->pushButton_settingsA->setIcon(QIcon(Graphics::getGraphicsPath()+"settings_gear.png"));
    ui->pushButton_settingsB->setIcon(QIcon(Graphics::getGraphicsPath()+"settings_gear.png"));
    ui->pushButton_settingsA->setStyleSheet("QPushButton{border:none;} QPushButton:hover{border:none;background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}");
    ui->pushButton_settingsB->setStyleSheet("QPushButton{border:none;} QPushButton:hover{border:none;background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}");

    animationForward = new QMovie(Graphics::getGraphicsPath()+"menu_burger_button.gif");
    animationBackward = new QMovie(Graphics::getGraphicsPath()+"menu_burger_button_backw.gif");
    ui->label_burgerAnim->setStyleSheet("QLabel:hover{background-color:"+Graphics::COLOR_WINDOW_CONTROL_HOVER+";}");

    QSize *size = new QSize(30, 30);
    animationForward->setScaledSize(*size);
    animationBackward->setScaledSize(*size);    

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

void WidgetFooter::setAppearance(bool isWide){
    this->isWide = isWide; //save the value if it is called from public
    if(!isWide){
        ui->pushButton_settingsA->show();
        ui->pushButton_settingsB->hide();
        ui->label_settings->setText("");
        //ui->pushButton_setSize->setIcon(QIcon(Graphics::getGraphicsPath()+"settings_horizontal.png"));

        ui->label_burgerAnim->setMovie(animationBackward);
        animationBackward->start();
    }else{
        ui->pushButton_settingsB->show();
        ui->pushButton_settingsA->hide();
        ui->label_settings->setText("Settings");
        //ui->pushButton_setSize->setIcon(QIcon(Graphics::getGraphicsPath()+"settings_vertical.png"));

        ui->label_burgerAnim->setMovie(animationForward);
        animationForward->start();
    }
}

