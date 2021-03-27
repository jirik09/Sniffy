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

void SyncPwmWindow::setSpecification(SyncPwmSpec *spec){
    this->spec = spec;
    if(spec->chans_depend){
        connect(settings->dialFreqCh[spec->drive_chx], &WidgetDialRange::valueChanged, this, &SyncPwmWindow::dialFreqCallback);
        connect(settings->dialFreqCh[spec->drive_chy], &WidgetDialRange::valueChanged, this, &SyncPwmWindow::dialFreqCallback);
        settings->greyOutComplementChanFreqDials(spec->driven_chx);
        settings->greyOutComplementChanFreqDials(spec->driven_chy);
    }
}

void SyncPwmWindow::restoreGUIAfterStartup(){
    uncheckStartButton();
    setStartTxt();
}

void SyncPwmWindow::setStartTxt(){
    settings->buttonStart->setText("START");
}

void SyncPwmWindow::setStopTxt(){
    settings->buttonStart->setText("STOP");
}

void SyncPwmWindow::uncheckStartButton(){
    settings->buttonStart->setChecked(false, 0);
}

void SyncPwmWindow::dialFreqCallback(float val, int chanIndex){
    if(chanIndex == spec->drive_chx){
        settings->dialFreqCh[spec->driven_chx]->setRealValue(val);
    }else {
        settings->dialFreqCh[spec->driven_chy]->setRealValue(val);
    }
}

SyncPwmWindow::~SyncPwmWindow(){
    delete ui;
}
