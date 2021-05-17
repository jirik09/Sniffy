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

    chart = new widgetChart(widget_chart, MAX_PATT_CHANNELS_NUM);
    chart->enableLocalMouseEvents(EventSelection::CLICKS_ONLY);
    verticalLayout_chart->addWidget(chart);

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600,300);
    ui->widget_module->setLayout(verticalLayout_chart);

    chartData = new QVector<QVector<QPointF>>;
    patternData = new QList<quint8>;

    settings = new PatternGeneratorSettings(verticalLayout_settings, config, this);
    patterns = new PatternGeneratorPatterns(this);
    painter = new PatternGeneratorPainter(chart, config, this);
    //fileLoader = new PatternGeneratorFileLoader();

    connect(settings->comboPatternSelection, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorWindow::patternSelectionChangedCallback);
    connect(settings->buttonStart, &WidgetButtons::clicked, this, &PatternGeneratorWindow::runGeneratorCallback);
    connect(settings->buttonSetDefault, &WidgetButtons::clicked, this, &PatternGeneratorWindow::restorePattern);

    connect(settings->buttonUserDefLoadPattern, &WidgetButtons::clicked, this, &PatternGeneratorWindow::openFileCallback);
    connect(settings->dialUserDefFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialCounterFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialBinaryCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialGrayCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialQuadratureFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->comboI2cClockFreq, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorWindow::freqChangedCombosCallback);

    connect(settings->dialUserDefLength, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);
    connect(settings->dialCounterLength, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);
    connect(settings->dialBinaryChanNum, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);
    connect(settings->dialGrayCodeChanNum, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);

    connect(chart, &widgetChart::mouseLeftClickEvent, this, &PatternGeneratorWindow::chartLeftClickCallback);
}

PatternGeneratorWindow::~PatternGeneratorWindow()
{
    delete ui;
}

void PatternGeneratorWindow::restoreGUIAfterStartup()
{
    settings->restoreSettingsAfterStartup();
    patternData = patterns->createPatternOrJustGetData(config->pattIndex, config->dataLen[config->pattIndex]);

    painter->repaint(patternData);
}

void PatternGeneratorWindow::setSpecification(PatternGeneratorSpec *spec)
{
    this->spec = spec;
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

QList<quint8> *PatternGeneratorWindow::getPatternData()
{   
    return patternData;
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

void PatternGeneratorWindow::restorePattern()
{
    patterns->setDefault(config->pattIndex);
}

/******************************** Common Callbacks *********************************/

void PatternGeneratorWindow::patternSelectionChangedCallback(int index)
{
    patternData = patterns->createPatternOrJustGetData(index, config->dataLen[index]);

    settings->showComponents(config->pattIndex, false);
    settings->showComponents(index, true);

    config->pattIndex = index;
    painter->repaint(patternData);
}

void PatternGeneratorWindow::freqChangedDialsCallback(float val)
{
    config->freq[config->pattIndex] = val;
    painter->repaint(patternData);
}

void PatternGeneratorWindow::freqChangedCombosCallback(int index, float realVal)
{
    Q_UNUSED(index);
    config->freq[config->pattIndex] = realVal;
    painter->repaint(patternData);
}

void PatternGeneratorWindow::dataLenChangedDialsCallback(float val)
{
    config->dataLen[config->pattIndex] = val;
    patternData = patterns->setDataLen(config->pattIndex, val);
    painter->repaint(patternData);
}

void PatternGeneratorWindow::chartLeftClickCallback(QGraphicsSceneMouseEvent *event)
{
    int dataLen = config->dataLen[config->pattIndex];
    int dataNum = (patterns->isExponencial()) ? qPow(2, dataLen) : dataLen;

    qreal height = chart->geometry().height();
    qreal width = chart->geometry().width();

    int position = ((event->pos().x() - 25) / (width - 44)) * dataNum;
    int channel = ((event->pos().y() - 23) / (height - 27)) * MAX_PATT_CHANNELS_NUM;

    if(position >= dataNum)
        position = dataNum - 1;

    patternData = patterns->modifyPattern(channel, position);

    painter->repaint(patternData);
}



