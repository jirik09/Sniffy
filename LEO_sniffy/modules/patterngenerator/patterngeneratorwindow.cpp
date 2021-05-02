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

    setting = new PatternGeneratorSettings(verticalLayout_settings, config, this);
    //fileLoader = new PatternGeneratorFileLoader();

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600,300);
    ui->widget_module->setLayout(verticalLayout_chart);

    chartData = new QVector<QVector<QPointF>>;   
}

PatternGeneratorWindow::~PatternGeneratorWindow()
{
    delete ui;
}


void PatternGeneratorWindow::restoreGUIAfterStartup()
{
    setting->restoreSettingsPanel();
}

void PatternGeneratorWindow::setSpecification(PatternGeneratorSpec *spec)
{
    this->spec = spec;
}
