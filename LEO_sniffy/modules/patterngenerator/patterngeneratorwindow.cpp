#include "patterngeneratorwindow.h"
#include "ui_patterngeneratorwindow.h"

PatternGeneratorWindow::PatternGeneratorWindow(PatternGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatternGeneratorWindow),
    config(config)
{
    ui->setupUi(this);

    QWidget *widget_settings = new QWidget(this, Qt::Window);
    QWidget *widget_chart = new QWidget(this, Qt::Window);
    QVBoxLayout *verticalLayout_chart = new QVBoxLayout();
    QVBoxLayout *verticalLayout_settings = new QVBoxLayout();
    widget_settings->setLayout(verticalLayout_settings);
    widget_chart->setLayout(verticalLayout_chart);

    verticalLayout_settings->setContentsMargins(4,4,4,4);
    verticalLayout_settings->setSpacing(2);

    widget_chart->setContentsMargins(0,0,0,0);
    verticalLayout_chart->setContentsMargins(0,0,0,0);
    verticalLayout_chart->setSpacing(0);

    chart = new widgetChart(widget_chart, 4);
    chart->setRange(0, 1, 0, 1);
    chart->setGraphColor(QColor(Graphics::COLOR_CHART_GRIDLEG_DEFAULT));
    verticalLayout_chart->addWidget(chart);

    settings = new PatternGeneratorSettings(verticalLayout_settings, config, this);
    //fileLoader = new PatternGeneratorFileLoader();

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600,300);
    ui->widget_module->setLayout(verticalLayout_chart);

    chartData = new QVector<QVector<QPointF>>;

    connect(settings->buttonUserDefLoadPattern, &WidgetButtons::clicked, this, &PatternGeneratorWindow::openFileCallback);
    connect(settings->buttonStart, &WidgetButtons::clicked, this, &PatternGeneratorWindow::runGeneratorCallback);
}

PatternGeneratorWindow::~PatternGeneratorWindow()
{
    delete ui;
}

void PatternGeneratorWindow::restoreGUIAfterStartup()
{
    settings->restoreSettingsAfterStartup();
}

void PatternGeneratorWindow::setSpecification(PatternGeneratorSpec *spec)
{
    this->spec = spec;
}

QList<QList<int>> *PatternGeneratorWindow::getPatternData() const
{
    return patternData;
}

void PatternGeneratorWindow::setProgress(int percent)
{
    setGenerateButton("Uploading "+ QString::number(percent)+ "%", Graphics::COLOR_WARNING);
}

void PatternGeneratorWindow::setGenerateButton(QString text, QString color)
{
    settings->buttonStart->setText(text);
    settings->buttonStart->setColor(color,0);
}

void PatternGeneratorWindow::setGeneratorState(bool onClick)
{
    if(config->state == PatternGeneratorConfig::State::UPLOADING){
        if(onClick){
            config->state = PatternGeneratorConfig::State::STOPPED;
            setGenerateButton("Start", Graphics::COLOR_CONTROLS);
            emit stopGenerator();
        }else{
            config->state = PatternGeneratorConfig::State::RUNNING;
            setGenerateButton("Stop", Graphics::COLOR_RUNNING);
        }
    }else if(config->state == PatternGeneratorConfig::State::RUNNING){
        config->state = PatternGeneratorConfig::State::STOPPED;
        setGenerateButton("Start", Graphics::COLOR_CONTROLS);
        emit stopGenerator();
    }else if(config->state == PatternGeneratorConfig::State::STOPPED){
        config->state = PatternGeneratorConfig::State::UPLOADING;
        setGenerateButton("Uploading",Graphics::COLOR_WARNING);
        emit runGenerator();
    }

    settings->enableGuiComponents(config->state==PatternGeneratorConfig::State::STOPPED);
}

void PatternGeneratorWindow::runGeneratorCallback()
{
    setGeneratorState(true);
}

void PatternGeneratorWindow::openFileCallback()
{
    QString fileName;
    int tmp = 0;
    fileName = QFileDialog::getOpenFileName(this,"Select input file","","Text files (*.csv *.txt)");
    /* TODO: creeate file loader */
}

