#include "syncpwmwindow.h"
#include "ui_syncpwmwindow.h"

SyncPwmWindow::SyncPwmWindow(SyncPwmConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyncPwmWindow),
    config(config)
{
    ui->setupUi(this);

    QSplitter *splitter = nullptr;
    QWidget *widget_chart = new QWidget(this, Qt::Window);
    QWidget *widget_settings = new QWidget(this, Qt::Window);
    QVBoxLayout *verticalLayout_chart = new QVBoxLayout();
    QVBoxLayout *verticalLayout_settings = new QVBoxLayout();

    widget_chart->setLayout(verticalLayout_chart);
    widget_settings->setLayout(verticalLayout_settings);
    widget_settings->setMaximumHeight(400);

    if(config->layout == Layout::HORIZONTAL){
        splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(widget_chart);
        splitter->addWidget(widget_settings);
    }else {
        splitter = new QSplitter(Qt::Vertical, this);
        splitter->addWidget(widget_settings);
        splitter->addWidget(widget_chart);
    }

    ui->horizontalLayout->addWidget(splitter);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    splitter->setContentsMargins(0,0,0,0);

    widget_chart->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    widget_chart->setContentsMargins(0,0,0,0);
    verticalLayout_chart->setContentsMargins(0,0,0,0);
    verticalLayout_chart->setSpacing(0);

    widget_settings->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    verticalLayout_settings->setContentsMargins(4,4,4,4);
    verticalLayout_settings->setSpacing(2);

    chart = new widgetChart(widget_chart, 4);
    //chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chart->setRange(0, 1, 0, RANGE_CHAN1_LOG1 + 1);
    verticalLayout_chart->addWidget(chart);

    settings = new SyncPwmSettings(verticalLayout_settings, config, this);
}

SyncPwmWindow::~SyncPwmWindow(){
    delete ui;
}
