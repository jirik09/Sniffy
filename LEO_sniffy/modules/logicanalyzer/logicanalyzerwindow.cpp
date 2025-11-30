#include "logicanalyzerwindow.h"
#include "ui_logicanalyzerwindow.h"
#include "../../GUI/widgetbuttons.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>

LogicAnalyzerWindow::LogicAnalyzerWindow(LogicAnalyzerConfig *conf, QWidget *parent) : QWidget(parent),
                                                                                       ui(new Ui::LogicAnalyzerWindow),
                                                                                       conf(conf)
{
    qDebug() << "LogicAnalyzerWindow::LogicAnalyzerWindow() - Start";
    ui->setupUi(this);

    // Use PatternChart to gain per-channel labeling similar to pattern generator
    chart = new PatternChart(ui->chartArea, 8);
    chart->setRange(0, 100, 0, 16); // Y range 0-16 for 8 channels (each 2 units high)
    chart->enableLocalMouseEvents(EventSelection::ALL);

    QVBoxLayout *layout = new QVBoxLayout(ui->chartArea);
    layout->addWidget(chart);
    layout->setContentsMargins(0, 0, 0, 0);

    // Assuming UI file has controlsArea
    if (ui->controlsArea->layout() == nullptr)
    {
        qDebug() << "LogicAnalyzerWindow::LogicAnalyzerWindow() - Creating controls";
        QHBoxLayout *controlsLayout = new QHBoxLayout(ui->controlsArea);

        // Trigger Mode Buttons (Scope-style: Stop/Single, Normal, Auto)
        buttonsTriggerMode = new WidgetButtons(this, 3, ButtonTypes::RADIO, "", 2);
        buttonsTriggerMode->setText("Single", 0); // Default state when stopped
        buttonsTriggerMode->setText("Normal", 1);
        buttonsTriggerMode->setText("Auto", 2);
        buttonsTriggerMode->setColor(Graphics::palette().warning, 0); // Orange for Stop/Single state
        controlsLayout->addWidget(buttonsTriggerMode);

        QComboBox *cbSampleRate = new QComboBox(this);
        cbSampleRate->addItem("10 kSPS", 10000);
        cbSampleRate->addItem("100 kSPS", 100000);
        cbSampleRate->addItem("1 MSPS", 1000000);
        cbSampleRate->addItem("2 MSPS", 2000000);
        cbSampleRate->addItem("3 MSPS", 3000000);
        cbSampleRate->addItem("5 MSPS", 5000000);
        cbSampleRate->addItem("7 MSPS", 7000000);
        cbSampleRate->addItem("10 MSPS", 10000000);
        cbSampleRate->addItem("13.3 MSPS", 13300000);
        cbSampleRate->addItem("15 MSPS", 15000000);
        cbSampleRate->addItem("17.3 MSPS", 17300000);
        cbSampleRate->addItem("20 MSPS", 20000000);
        // Set default to 1 MHz (index 2)
        cbSampleRate->setCurrentIndex(2);
        activeSampleRate = 1000000.0;

        controlsLayout->addWidget(cbSampleRate);

        // Trigger Controls
        QLabel *lblTrigCh = new QLabel("Trig Ch:", this);
        QComboBox *cbTrigCh = new QComboBox(this);
        for (int i = 1; i <= 8; ++i)
            cbTrigCh->addItem(QString::number(i), i);

        QLabel *lblTrigEdge = new QLabel("Edge:", this);
        QComboBox *cbTrigEdge = new QComboBox(this);
        cbTrigEdge->addItem("Rising", 0);
        cbTrigEdge->addItem("Falling", 1);

        controlsLayout->addWidget(lblTrigCh);
        controlsLayout->addWidget(cbTrigCh);
        controlsLayout->addWidget(lblTrigEdge);
        controlsLayout->addWidget(cbTrigEdge);

        connect(buttonsTriggerMode, &WidgetButtons::clicked, this, &LogicAnalyzerWindow::triggerModeCallback);

        connect(cbSampleRate, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbSampleRate](int index)
            { emit sampleRateChanged(cbSampleRate->itemData(index).toInt()); });
        connect(cbTrigCh, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbTrigCh](int index)
            { emit triggerChannelChanged(cbTrigCh->itemData(index).toInt()); });
        connect(cbTrigEdge, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbTrigEdge](int index)
            { emit triggerEdgeChanged(cbTrigEdge->itemData(index).toInt()); });

        // Push initial values to FW so first Start works without user changes
        emit sampleRateChanged(cbSampleRate->currentData().toInt());
        emit triggerChannelChanged(cbTrigCh->currentData().toInt());
        emit triggerEdgeChanged(cbTrigEdge->currentData().toInt());

        // Pause streaming display (does not send STOP to device)
        QCheckBox *chkPause = new QCheckBox("Pause", this);
        controlsLayout->addWidget(chkPause);
        connect(chkPause, &QCheckBox::toggled, [this](bool on)
                { paused = on; });

        // Decimation factor (every Nth sample)
        QLabel *lblDecim = new QLabel("Decim:", this);
        QSpinBox *spDecim = new QSpinBox(this);
        spDecim->setRange(1, 512);
        spDecim->setValue(1);
        controlsLayout->addWidget(lblDecim);
        controlsLayout->addWidget(spDecim);
        connect(spDecim, QOverload<int>::of(&QSpinBox::valueChanged), [this](int v)
                { decimationFactor = (v < 1 ? 1 : v); });
    }
    qDebug() << "LogicAnalyzerWindow::LogicAnalyzerWindow() - End";
}

LogicAnalyzerWindow::~LogicAnalyzerWindow()
{
    qDebug() << "LogicAnalyzerWindow::~LogicAnalyzerWindow()";
    delete ui;
}

void LogicAnalyzerWindow::showData(const QVector<QVector<int>> &data, double sampleRate)
{
    qDebug() << "LogicAnalyzerWindow::showData() - Start. New block samples:" << (data.isEmpty() ? 0 : data[0].size());
    if (!chart || data.isEmpty() || paused)
        return;

    if (activeSampleRate <= 0.0)
        activeSampleRate = sampleRate; // latch initial rate
    double timeStep = 1.0 / activeSampleRate;
    int blockSamples = data[0].size();
    double baseTime = totalSamples * timeStep;

    for (int ch = 0; ch < 8 && ch < data.size(); ++ch)
    {
        const auto &channelData = data[ch];
        QVector<QPointF> appendPoints;
        appendPoints.reserve(channelData.size() * 2);
        for (int i = 0; i < channelData.size(); ++i)
        {
            if (decimationFactor > 1 && (i % decimationFactor) != 0)
                continue;
            double t = baseTime + i * timeStep;
            double val = channelData[i] * 0.8 + (ch * 2.0);
            if (i > 0)
            {
                // For decimation we need previous displayed sample, may not be i-1
                // Simple approach: duplicate current val for step edge if sparse
                double prevVal = channelData[i] * 0.8 + (ch * 2.0);
                appendPoints.append(QPointF(t, prevVal));
            }
            appendPoints.append(QPointF(t, val));
        }
        if (totalSamples == 0)
        {
            // first block: create trace
            chart->updateTrace(&appendPoints, ch);
        }
        else
        {
            chart->appendToTrace(ch, &appendPoints);
        }
    }

    totalSamples += blockSamples;
    double endT = totalSamples * timeStep;
    chart->setRangeX(0, endT);
    qDebug() << "LogicAnalyzerWindow::showData() - End. Total samples:" << totalSamples;
}

void LogicAnalyzerWindow::setSpecification(const QList<QString> &channelNames)
{
    specChannelNames = channelNames;
    // If using PatternChart, apply channel names immediately
    if (auto *pattern = dynamic_cast<PatternChart *>(chart))
    {
        QStringList names;
        for (const auto &n : specChannelNames)
            names << n;
        pattern->setChannelNames(names);
    }
}

void LogicAnalyzerWindow::resetStream()
{
    totalSamples = 0;
    if (chart)
        chart->clearAll();
}

void LogicAnalyzerWindow::singleSamplingDone()
{
    // Mirror Scope behavior: after a single batch, revert button 0 to "Single"
    if (buttonsTriggerMode)
    {
        buttonsTriggerMode->setColor(Graphics::palette().warning, 0);
        buttonsTriggerMode->setText("Single", 0);
    }
}

void LogicAnalyzerWindow::triggerModeCallback(int index)
{
    // 0: Stop/Single, 1: Normal, 2: Auto
    // Mapped to ScopeTriggerMode: 0=Stop, 1=Normal, 2=Auto, 4=Single
    // But we use a simplified int mapping for the signal: 0=Auto, 1=Normal, 2=Single, 3=Stop

    if (index == 0)
    {
        if (buttonsTriggerMode->getText(0) == "Stop")
        {
            // User clicked Stop -> Stop capture
            buttonsTriggerMode->setColor(Graphics::palette().warning, 0);
            emit triggerModeChanged(3); // Stop
            buttonsTriggerMode->setText("Single", 0);
        }
        else if (buttonsTriggerMode->getText(0) == "Single")
        {
            // User clicked Single -> Start Single Shot
            emit triggerModeChanged(2); // Single
            buttonsTriggerMode->setColor(Graphics::palette().running, 0);
            buttonsTriggerMode->setText("Stop", 0);
        }
    }
    else if (index == 1)
    {
        // Normal
        emit triggerModeChanged(1); // Normal
        buttonsTriggerMode->setColor(Graphics::palette().warning, 0);
        buttonsTriggerMode->setText("Stop", 0);
    }
    else if (index == 2)
    {
        // Auto
        emit triggerModeChanged(0); // Auto
        buttonsTriggerMode->setColor(Graphics::palette().warning, 0);
        buttonsTriggerMode->setText("Stop", 0);
    }
}
