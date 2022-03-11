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

    painter = new PatternGeneratorPainter(config, this);

    if(PATTERNS_USE_QCUSTOM_PLOT)
        verticalLayout_chart->addWidget(painter->plot);
    else
        verticalLayout_chart->addWidget(painter->chart);    

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600,300);
    ui->widget_module->setLayout(verticalLayout_chart);

    patternData = new QList<patttype>;

    settings = new PatternGeneratorSettings(verticalLayout_settings, config, this);
    patterns = new PatternGeneratorPatterns(this);    
    //fileLoader = new PatternGeneratorFileLoader();

    connect(settings->comboPatternSelection, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorWindow::patternSelectionChangedCallback);
    connect(settings->buttonStart, &WidgetButtons::clicked, this, &PatternGeneratorWindow::runGeneratorCallback);
    connect(settings->buttonSetDefault, &WidgetButtons::clicked, this, &PatternGeneratorWindow::resetPatternCallback);

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

    connect(settings->comboQuadratureSeqAbba, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorWindow::quadratureSequenceChangedCallback);

    if(!PATTERNS_USE_QCUSTOM_PLOT)
        connect(painter->chart, &widgetChart::mouseLeftClickEvent, this, &PatternGeneratorWindow::chartEditDataOnLeftClickCallback);
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
    settings->setSpecification(spec);
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

QList<patttype> *PatternGeneratorWindow::getPatternData()
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
    fileName = QFileDialog::getOpenFileName(this,"Select input file","","Text files (*.csv *.txt)");
    /* TODO: creeate file loader */
}

void PatternGeneratorWindow::resetPatternCallback()
{
    patternData = patterns->setDefault(config->pattIndex);
    painter->repaint(patternData);
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

void PatternGeneratorWindow::quadratureSequenceChangedCallback(int index)
{    
    patternData = patterns->setQuadratureSequence(index==0);
    painter->repaint(patternData);
}

void PatternGeneratorWindow::chartEditDataOnLeftClickCallback(QGraphicsSceneMouseEvent *event)
{
    int dataLen = config->dataLen[config->pattIndex];
    int dataNum = (patterns->isExponencial()) ? qPow(2, dataLen) : dataLen;

    qreal height = painter->chart->geometry().height();
    qreal width = painter->chart->geometry().width();

    int position = ((event->pos().x() - 25) / (width - 44)) * dataNum;
    int channel = ((event->pos().y() - 20) / (height - 30)) * PATT_MAX_CHANNELS_NUM;

    if(position >= dataNum)
        position = dataNum - 1;
    else if(position < 0)
        position = 0;

    patternData = patterns->modifyPattern(channel, position);

    painter->repaint(patternData);
}



