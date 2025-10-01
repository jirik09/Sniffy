#include "patterngeneratorwindow.h"
#include "ui_patterngeneratorwindow.h"

PatternGeneratorWindow::PatternGeneratorWindow(PatternGeneratorConfig *config, QWidget *parent) : QWidget(parent),
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

    verticalLayout_settings->setContentsMargins(4, 4, 4, 4);
    verticalLayout_settings->setSpacing(2);

    widget_chart->setContentsMargins(0, 0, 0, 0);
    verticalLayout_chart->setContentsMargins(0, 0, 0, 0);
    verticalLayout_chart->setSpacing(0);

    painter = new PatternGeneratorPainter(config, this);

    if (PATTERNS_USE_QCUSTOM_PLOT)
        verticalLayout_chart->addWidget(painter->plot);
    else
        verticalLayout_chart->addWidget(painter->chart);

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600, 300);
    ui->widget_module->setLayout(verticalLayout_chart);

    patternData = new QList<patttype>;

    settings = new PatternGeneratorSettings(verticalLayout_settings, config, this);
    patterns = new PatternGeneratorPatterns(this);
    // fileLoader = new PatternGeneratorFileLoader();

    connect(settings->comboPatternSelection, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorWindow::patternSelectionChangedCallback);
    connect(settings->buttonStart, &WidgetButtons::clicked, this, &PatternGeneratorWindow::runGeneratorCallback);
    connect(settings->buttonSetDefault, &WidgetButtons::clicked, this, &PatternGeneratorWindow::resetPatternCallback);

    connect(settings->buttonUserDefLoadPattern, &WidgetButtons::clicked, this, &PatternGeneratorWindow::openFileCallback);
    connect(settings->dialUserDefFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialCounterFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialBinaryCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialGrayCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    connect(settings->dialQuadratureFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    // I2C combo wiring will be connected below after widgets exist

    // New patterns wiring
    if (settings->dialPrbsFreq)
        connect(settings->dialPrbsFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->comboPrbsOrder)
        connect(settings->comboPrbsOrder, &WidgetSelection::selectedIndexChanged, this, [this](int idx, float real)
                { Q_UNUSED(real); Q_UNUSED(idx); patternData = patterns->setPrbsOrder(settings->comboPrbsOrder->getSelectedValue()); painter->repaint(patternData); });
    if (settings->dialPwmFreq)
        connect(settings->dialPwmFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->dialPwmDuty)
        connect(settings->dialPwmDuty, &WidgetDialRange::valueChanged, this, [this](float val)
                { patternData = patterns->setPwmDuty((int)val); painter->repaint(patternData); });
    if (settings->dialLineCodeFreq)
        connect(settings->dialLineCodeFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->comboLineCodeType)
        connect(settings->comboLineCodeType, &WidgetSelection::selectedIndexChanged, this, [this](int idx, float real)
                { Q_UNUSED(real); patternData = patterns->setLineCodeType(static_cast<LineCodeType>(settings->comboLineCodeType->getSelectedValue())); painter->repaint(patternData); });
    if (settings->dial4b5bFreq)
        connect(settings->dial4b5bFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->dial4b5bGroups)
        connect(settings->dial4b5bGroups, &WidgetDialRange::valueChanged, this, [this](float val)
                { patternData = patterns->setFourBFiveBGroups((int)val); painter->repaint(patternData); });
    if (settings->dialJohnsonFreq)
        connect(settings->dialJohnsonFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->dialJohnsonPhases)
        connect(settings->dialJohnsonPhases, &WidgetDialRange::valueChanged, this, [this](float val)
                { patternData = patterns->setJohnsonPhases((int)val); painter->repaint(patternData); });
    if (settings->dialPdmFreq)
        connect(settings->dialPdmFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->dialPdmLevel)
        connect(settings->dialPdmLevel, &WidgetDialRange::valueChanged, this, [this](float val)
                { patternData = patterns->setPdmLevel((int)val); painter->repaint(patternData); });
    if (settings->dialParBusFreq)
        connect(settings->dialParBusFreq, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::freqChangedDialsCallback);
    if (settings->dialParBusWidth)
        connect(settings->dialParBusWidth, &WidgetDialRange::valueChanged, this, [this](float val)
                { patternData = patterns->setParBusWidth((int)val); painter->repaint(patternData); });

    // UART wiring
    if (settings->comboUartBaud)
        connect(settings->comboUartBaud, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { this->config->freq[this->config->pattIndex] = real; patternData = patterns->setUartBaud((int)real); painter->repaint(patternData); });
    if (settings->dialUartDataBits)
        connect(settings->dialUartDataBits, &WidgetDialRange::valueChanged, this, [this](float v)
                { patternData = patterns->setUartDataBits((int)v); painter->repaint(patternData); });
    if (settings->comboUartParity)
        connect(settings->comboUartParity, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setUartParity(static_cast<UartParity>((int)real)); painter->repaint(patternData); });
    if (settings->comboUartStopBits)
        connect(settings->comboUartStopBits, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setUartStopBits((int)real); painter->repaint(patternData); });
    if (settings->comboUartBitOrder)
        connect(settings->comboUartBitOrder, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setUartBitOrder(static_cast<BitOrder>((int)real)); painter->repaint(patternData); });
    if (settings->comboUartIdle)
        connect(settings->comboUartIdle, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setUartIdle(static_cast<UartIdleLevel>((int)real)); painter->repaint(patternData); });
    if (settings->comboUartFramingErr)
        connect(settings->comboUartFramingErr, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setUartFramingError(((int)real) != 0); painter->repaint(patternData); });
    if (settings->comboUartBreak)
        connect(settings->comboUartBreak, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setUartBreak(((int)real) != 0); painter->repaint(patternData); });

    // SPI wiring
    if (settings->dialSpiClockFreq)
        connect(settings->dialSpiClockFreq, &WidgetDialRange::valueChanged, this, [this](float real)
                { this->config->freq[this->config->pattIndex] = real; patternData = patterns->setSpiClockHz((int)real); painter->repaint(patternData); });
    if (settings->comboSpiMode)
        connect(settings->comboSpiMode, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setSpiMode(static_cast<SpiMode>((int)real)); painter->repaint(patternData); });
    if (settings->dialSpiWordSize)
        connect(settings->dialSpiWordSize, &WidgetDialRange::valueChanged, this, [this](float v)
                { patternData = patterns->setSpiWordSize((int)v); painter->repaint(patternData); });
    if (settings->comboSpiBitOrder)
        connect(settings->comboSpiBitOrder, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setSpiBitOrder(static_cast<BitOrder>((int)real)); painter->repaint(patternData); });
    if (settings->comboSpiCsGating)
        connect(settings->comboSpiCsGating, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setSpiCsGating(((int)real) != 0); painter->repaint(patternData); });
    if (settings->dialSpiPauseTicks)
        connect(settings->dialSpiPauseTicks, &WidgetDialRange::valueChanged, this, [this](float v)
                { patternData = patterns->setSpiPauseTicks((int)v); painter->repaint(patternData); });

    // I2C wiring
    if (settings->comboI2cClockFreq)
        connect(settings->comboI2cClockFreq, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { this->config->freq[this->config->pattIndex] = real; patternData = patterns->setI2cSpeed((int)real); painter->repaint(patternData); });
    if (settings->comboI2cCommType)
        connect(settings->comboI2cCommType, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setI2cReadWrite((ModeRW)((int)real)); painter->repaint(patternData); });
    if (settings->comboI2cAddrMode)
        connect(settings->comboI2cAddrMode, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setI2cAddrMode(static_cast<I2cAddrMode>((int)real)); painter->repaint(patternData); });
    if (settings->dialI2cAddress)
        connect(settings->dialI2cAddress, &WidgetDialRange::valueChanged, this, [this](float v)
                { patternData = patterns->setI2cAddress((int)v); painter->repaint(patternData); });
    if (settings->comboI2cAck)
        connect(settings->comboI2cAck, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setI2cAck(((int)real) != 0); painter->repaint(patternData); });
    if (settings->comboI2cStretch)
        connect(settings->comboI2cStretch, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setI2cStretch(((int)real) != 0); painter->repaint(patternData); });
    if (settings->comboI2cRepStart)
        connect(settings->comboI2cRepStart, &WidgetSelection::selectedIndexChanged, this, [this](int, float real)
                { patternData = patterns->setI2cRepeatedStart(((int)real) != 0); painter->repaint(patternData); });

    connect(settings->dialUserDefLength, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);
    connect(settings->dialCounterLength, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);
    connect(settings->dialBinaryChanNum, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);
    connect(settings->dialGrayCodeChanNum, &WidgetDialRange::valueChanged, this, &PatternGeneratorWindow::dataLenChangedDialsCallback);

    connect(settings->comboQuadratureSeqAbba, &WidgetSelection::selectedIndexChanged, this, &PatternGeneratorWindow::quadratureSequenceChangedCallback);

    if (!PATTERNS_USE_QCUSTOM_PLOT)
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
    setGenerateButton("Uploading " + QString::number(percent) + "%", Graphics::palette().warning);
}

void PatternGeneratorWindow::setGenerateButton(QString text, QString color)
{
    settings->buttonStart->setText(text);
    settings->buttonStart->setColor(color, 0);
}

void PatternGeneratorWindow::setGeneratorState(bool onClick)
{
    if (config->state == PatternGeneratorConfig::State::UPLOADING)
    {
        if (onClick)
        {
            config->state = PatternGeneratorConfig::State::STOPPED;
            setGenerateButton("Start", Graphics::palette().controls);
            emit stopGenerator();
        }
        else
        {
            config->state = PatternGeneratorConfig::State::RUNNING;
            setGenerateButton("Stop", Graphics::palette().running);
        }
    }
    else if (config->state == PatternGeneratorConfig::State::RUNNING)
    {
        config->state = PatternGeneratorConfig::State::STOPPED;
        setGenerateButton("Start", Graphics::palette().controls);
        emit stopGenerator();
    }
    else if (config->state == PatternGeneratorConfig::State::STOPPED)
    {
        config->state = PatternGeneratorConfig::State::UPLOADING;
        setGenerateButton("Uploading", Graphics::palette().warning);
        emit runGenerator();
    }

    settings->enableGuiComponents(config->state == PatternGeneratorConfig::State::STOPPED);
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
    fileName = QFileDialog::getOpenFileName(this, "Select input file", "", "Text files (*.csv *.txt)");
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
    patternData = patterns->setQuadratureSequence(index == 0);
    painter->repaint(patternData);
}

void PatternGeneratorWindow::chartEditDataOnLeftClickCallback(QGraphicsSceneMouseEvent *event)
{
    // Use actual rendered data length to align edit with visualization
    int dataNum = (patternData != nullptr) ? patternData->length() : 0;
    if (dataNum <= 0)
        return;

    qreal nx = 0.0, ny = 0.0;
    painter->chart->mapSceneToPlotNormalized(event->scenePos(), nx, ny);

    int position = static_cast<int>(nx * dataNum);
    if (position < 0)
        position = 0;
    if (position >= dataNum)
        position = dataNum - 1;

    int channel = static_cast<int>(ny * PATT_MAX_CHANNELS_NUM);
    if (channel < 0)
        channel = 0;
    if (channel >= PATT_MAX_CHANNELS_NUM)
        channel = PATT_MAX_CHANNELS_NUM - 1;

    patternData = patterns->modifyPattern(channel, position);

    painter->repaint(patternData);
}
