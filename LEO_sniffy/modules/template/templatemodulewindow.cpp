#include "templatemodulewindow.h"
#include "ui_templatemodulewindow.h"

TemplateModuleWindow::TemplateModuleWindow(TemplateModuleConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TemplateModuleWindow),
    config(config)
{
    ui->setupUi(this);
    //TODO setup the GUI here
}

TemplateModuleWindow::~TemplateModuleWindow()
{
    delete ui;
}


void TemplateModuleWindow::restoreGUIAfterStartup()
{
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}
