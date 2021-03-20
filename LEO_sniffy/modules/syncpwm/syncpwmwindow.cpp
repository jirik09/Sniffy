#include "syncpwmwindow.h"
#include "ui_syncpwmwindow.h"

SyncPwmWindow::SyncPwmWindow(SyncPwmConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncPwmWindow),
    config(config)
{
    ui->setupUi(this);

    chart = new widgetChart(ui->widget_chart, 4);
    chart->setRange(0, 1, 0, RANGE_CHAN1_LOG1 + 1);
    ui->verticalLayout_chart->addWidget(chart);

    settings = new SyncPwmSettings(ui->verticalLayout_settings, this);
}

SyncPwmWindow::~SyncPwmWindow()
{
    delete ui;
}
