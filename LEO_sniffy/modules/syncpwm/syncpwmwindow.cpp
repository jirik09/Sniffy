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
    widget_chart->setContentsMargins(4,4,4,0);
    widget_settings->setLayout(verticalLayout_settings);

    QList<int> sizes;
    if(config->layout == Layout::HORIZONTAL)
    {
        splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(widget_chart);
        splitter->addWidget(widget_settings);

        sizes = splitter->sizes();
        int wdth = splitter->width();
        sizes = {wdth*2, wdth};

    }else {
        widget_settings->setMaximumHeight(420);
        widget_settings->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);

        splitter = new QSplitter(Qt::Vertical, this);
        splitter->addWidget(widget_settings);
        splitter->addWidget(widget_chart);

        sizes = splitter->sizes();
        int hght = splitter->height();
        sizes = {hght, hght*2};
    }

    splitter->setSizes(sizes);
    ui->horizontalLayout->addWidget(splitter);

    verticalLayout_chart->setContentsMargins(0,0,0,4);
    verticalLayout_chart->setSpacing(0);
    verticalLayout_settings->setContentsMargins(4,1,4,2);
    verticalLayout_settings->setSpacing(1);

    chart = new widgetChart(widget_chart, CHANNELS_NUM);
    verticalLayout_chart->addWidget(chart);

    settings = new SyncPwmSettings(verticalLayout_settings, config, this);
    painter = new SyncPwmPainter(chart, config, this);
}

void SyncPwmWindow::connectDependentChannels(){
    connect(settings->dialFreqCh[spec->drive_chx], &WidgetDialRange::valueChanged, this, &SyncPwmWindow::dialFreqCallback);
    connect(settings->dialFreqCh[spec->drive_chy], &WidgetDialRange::valueChanged, this, &SyncPwmWindow::dialFreqCallback);
}

void SyncPwmWindow::disconnectDependentChannels(){
    disconnect(settings->dialFreqCh[spec->drive_chx], &WidgetDialRange::valueChanged, this, &SyncPwmWindow::dialFreqCallback);
    disconnect(settings->dialFreqCh[spec->drive_chy], &WidgetDialRange::valueChanged, this, &SyncPwmWindow::dialFreqCallback);
}

void SyncPwmWindow::setSpecification(SyncPwmSpec *spec){
    this->spec = spec;
    settings->setSpecification(spec);

    if(spec->chans_depend){
        connectDependentChannels();
        settings->greyOutComplementChanFreqDials(spec->driven_chx);
        settings->greyOutComplementChanFreqDials(spec->driven_chy);
    }

    for(int i = 0; i < CHANNELS_NUM; i++){
        settings->separator[i]->setText("Channel " + QString::number(i+1) + " | " + spec->pinsList[i]);
    }
}

void SyncPwmWindow::restoreGUIAfterStartup(){
    uncheckStartButton();
    uncheckEquidistantButton();
    setStartTxt();
}

void SyncPwmWindow::setStartTxt(){
    settings->buttonStart->setText("Start");
    settings->buttonStart->setColor(Graphics::COLOR_CONTROLS,0);
}

void SyncPwmWindow::setStopTxt(){
    settings->buttonStart->setText("Stop");
    settings->buttonStart->setColor(Graphics::COLOR_RUNNING,0);
}

void SyncPwmWindow::uncheckStartButton(){
    settings->buttonStart->setChecked(false, 0);
}

void SyncPwmWindow::uncheckEquidistantButton(){
    settings->buttonEquidist->setChecked(false, 0);
}

void SyncPwmWindow::dialFreqCallback(float val, int chanIndex){
    if(chanIndex == spec->drive_chx){
        settings->dialFreqCh[spec->driven_chx]->setRealValue(val, false);
    }else {
        settings->dialFreqCh[spec->driven_chy]->setRealValue(val, false);
    }
}

void SyncPwmWindow::setFreqDial(float val, int chanIndex){
    settings->dialFreqCh[chanIndex]->setRealValue(val, true);
}

void SyncPwmWindow::setPhaseDial(float val, int chanIndex){
    settings->dialPhaseCh[chanIndex]->setRealValue(val, true);
}

void SyncPwmWindow::setDutyDial(float val, int chanIndex){
    settings->dialDutyCh[chanIndex]->setRealValue(val, true);
}

void SyncPwmWindow::enableChannel(bool enable,int chanIndex){
    painter->enableChannel(enable, chanIndex);
}

void SyncPwmWindow::repaint(){
    painter->repaint();
}

SyncPwmWindow::~SyncPwmWindow(){
    delete ui;
}
