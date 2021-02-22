#include "voltmeterwindow.h"
#include "ui_voltmeterwindow.h"

VoltmeterWindow::VoltmeterWindow(VoltmeterConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoltmeterWindow),
    config(config)
{
    ui->setupUi(this);
    //TODO setup the GUI here
}

VoltmeterWindow::~VoltmeterWindow()
{
    delete ui;
}


void VoltmeterWindow::restoreGUIAfterStartup()
{
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}
