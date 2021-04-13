#include "pwmgeneratorwindow.h"
#include "ui_pwmgeneratorwindow.h"

PWMGeneratorWindow::PWMGeneratorWindow(PWMGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PWMGeneratorWindow),
    config(config)
{
    ui->setupUi(this);
    setStyleSheet("QWidget{background-color:"+Graphics::COLOR_WINDOW_WIDGET+";}");
    //TODO setup the GUI here
}

PWMGeneratorWindow::~PWMGeneratorWindow()
{
    delete ui;
}


void PWMGeneratorWindow::restoreGUIAfterStartup()
{
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}
