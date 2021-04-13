#include "voltagesourcewindow.h"
#include "ui_voltagesourcewindow.h"

VoltageSourceWindow::VoltageSourceWindow(VoltageSourceConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoltageSourceWindow),
    config(config)
{
    ui->setupUi(this);
    setStyleSheet("QWidget{background-color:"+Graphics::COLOR_WINDOW_WIDGET+";}");
    //TODO setup the GUI here
}

VoltageSourceWindow::~VoltageSourceWindow()
{
    delete ui;
}


void VoltageSourceWindow::restoreGUIAfterStartup()
{
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}
