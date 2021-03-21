#include "arbgeneratorwindow.h"
#include "ui_arbgeneratorwindow.h"

ArbGeneratorWindow::ArbGeneratorWindow(ArbGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArbGeneratorWindow),
    config(config)
{
    ui->setupUi(this);
    //TODO setup the GUI here
}

ArbGeneratorWindow::~ArbGeneratorWindow()
{
    delete ui;
}


void ArbGeneratorWindow::restoreGUIAfterStartup()
{
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}
