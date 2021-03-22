#include "arbgeneratorwindow.h"
#include "ui_arbgeneratorwindow.h"

ArbGeneratorWindow::ArbGeneratorWindow(ArbGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArbGeneratorWindow),
    config(config)
{
    ui->setupUi(this);
    //TODO setup the GUI here

    QWidget *widget_settings = new QWidget(this, Qt::Window);
    QVBoxLayout *verticalLayout_settings = new QVBoxLayout();
    widget_settings->setLayout(verticalLayout_settings);

    widget_settings->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    verticalLayout_settings->setContentsMargins(4,4,4,4);
    verticalLayout_settings->setSpacing(2);

    setting = new ArbGenPanelSettings(verticalLayout_settings,this);

    ui->widget_settings->setLayout(verticalLayout_settings);

    connect(setting,&ArbGenPanelSettings::signalChanged,this,&ArbGeneratorWindow::generateSignalCallback);

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

void ArbGeneratorWindow::generateSignalCallback()
{
    for (int i = 0;i <MAX_ARB_CHANNELS_NUM ;i++ ) {
        if(setting->channelEnabled[i]){
            qDebug () <<"generate ch"<<i<<setting->dialFreqCh[i]->getRealValue()<<setting->dialAmplitudeCh[i]->getRealValue();
        }
    }

}

