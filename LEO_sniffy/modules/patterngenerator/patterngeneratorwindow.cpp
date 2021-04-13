#include "patterngeneratorwindow.h"
#include "ui_patterngeneratorwindow.h"

PatternGeneratorWindow::PatternGeneratorWindow(PatternGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatternGeneratorWindow),
    config(config)
{
    ui->setupUi(this);
    setStyleSheet("QWidget{background-color:"+Graphics::COLOR_WINDOW_WIDGET+";}");
    //TODO setup the GUI here
}

PatternGeneratorWindow::~PatternGeneratorWindow()
{
    delete ui;
}


void PatternGeneratorWindow::restoreGUIAfterStartup()
{
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}
