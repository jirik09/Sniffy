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

        WidgetButtons *btnsControl = new WidgetButtons(this, 2, ButtonTypes::NORMAL);
        btnsControl->setText("Start", 0);
        btnsControl->setText("Stop", 1);

        QComboBox *cbSampleRate = new QComboBox(this);
        cbSampleRate->addItem("10 kHz", 10000);
        cbSampleRate->addItem("100 kHz", 100000);
        cbSampleRate->addItem("1 MHz", 1000000);
        cbSampleRate->addItem("10 MHz", 10000000);

        controlsLayout->addWidget(btnsControl);
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

        connect(btnsControl, &WidgetButtons::clicked, [this](int index)
                {
            if (index == 0) emit startCapture();
            else emit stopCapture(); });
        connect(cbSampleRate, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbSampleRate](int index)
                { emit sampleRateChanged(cbSampleRate->itemData(index).toInt()); });
        connect(cbTrigCh, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbTrigCh](int index)
                { emit triggerChannelChanged(cbTrigCh->itemData(index).toInt()); });
        connect(cbTrigEdge, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbTrigEdge](int index)
                { emit triggerEdgeChanged(cbTrigEdge->itemData(index).toInt()); });

        // Stream mode toggle (Batch / Continuous)
        QLabel *lblMode = new QLabel("Mode:", this);
        QComboBox *cbMode = new QComboBox(this);
        cbMode->addItem("Batch", 0);
        cbMode->addItem("Continuous", 1);
        controlsLayout->addWidget(lblMode);
        controlsLayout->addWidget(cbMode);
        connect(cbMode, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbMode](int index)
                {
            bool cont = cbMode->itemData(index).toInt() == 1;
            continuousMode = cont;
            emit streamModeChanged(cont); });

        // Auto-scroll enable
        QCheckBox *chkAutoScroll = new QCheckBox("AutoScroll", this);
        chkAutoScroll->setChecked(true);
        controlsLayout->addWidget(chkAutoScroll);
        connect(chkAutoScroll, &QCheckBox::toggled, [this](bool on)
                { autoScrollEnabled = on; });

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

        // View window (ms) for auto-scroll; 0 => full
        QLabel *lblWindow = new QLabel("Window(ms):", this);
        QSpinBox *spWindow = new QSpinBox(this);
        spWindow->setRange(0, 60000);
        spWindow->setValue(0);
        controlsLayout->addWidget(lblWindow);
        controlsLayout->addWidget(spWindow);
        connect(spWindow, QOverload<int>::of(&QSpinBox::valueChanged), [this](int v)
                { viewWindowSeconds = v / 1000.0; });
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
    if (autoScrollEnabled && viewWindowSeconds > 0.0)
    {
        double startT = endT - viewWindowSeconds;
        if (startT < 0)
            startT = 0;
        chart->setRangeX(startT, endT);
    }
    else
    {
        chart->setRangeX(0, endT);
    }
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
    activeSampleRate = 0.0;
    if (chart)
        chart->clearAll();
}
