#include "logicanalyzerwindow.h"
#include "ui_logicanalyzerwindow.h"
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDebug>

LogicAnalyzerWindow::LogicAnalyzerWindow(LogicAnalyzerConfig *conf, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogicAnalyzerWindow),
    conf(conf)
{
    qDebug() << "LogicAnalyzerWindow::LogicAnalyzerWindow() - Start";
    ui->setupUi(this);

    chart = new widgetChart(ui->chartArea, 8);
    chart->setRange(0, 100, 0, 16); // Y range 0-16 for 8 channels (each 2 units high)
    chart->enableLocalMouseEvents(EventSelection::ALL);
    
    QVBoxLayout *layout = new QVBoxLayout(ui->chartArea);
    layout->addWidget(chart);
    layout->setContentsMargins(0,0,0,0);

    // Assuming UI file has controlsArea
    if (ui->controlsArea->layout() == nullptr) {
        qDebug() << "LogicAnalyzerWindow::LogicAnalyzerWindow() - Creating controls";
        QHBoxLayout *controlsLayout = new QHBoxLayout(ui->controlsArea);
        
        QPushButton *btnStart = new QPushButton("Start", this);
        QPushButton *btnStop = new QPushButton("Stop", this);
        
        QComboBox *cbSampleRate = new QComboBox(this);
        cbSampleRate->addItem("10 kHz", 10000);
        cbSampleRate->addItem("100 kHz", 100000);
        cbSampleRate->addItem("1 MHz", 1000000);
        cbSampleRate->addItem("10 MHz", 10000000);
        
        controlsLayout->addWidget(btnStart);
        controlsLayout->addWidget(btnStop);
        controlsLayout->addWidget(cbSampleRate);
        
        // Trigger Controls
        QLabel *lblTrigCh = new QLabel("Trig Ch:", this);
        QComboBox *cbTrigCh = new QComboBox(this);
        for(int i=1; i<=8; ++i) cbTrigCh->addItem(QString::number(i), i);
        
        QLabel *lblTrigEdge = new QLabel("Edge:", this);
        QComboBox *cbTrigEdge = new QComboBox(this);
        cbTrigEdge->addItem("Rising", 0);
        cbTrigEdge->addItem("Falling", 1);

        controlsLayout->addWidget(lblTrigCh);
        controlsLayout->addWidget(cbTrigCh);
        controlsLayout->addWidget(lblTrigEdge);
        controlsLayout->addWidget(cbTrigEdge);

        connect(btnStart, &QPushButton::clicked, this, &LogicAnalyzerWindow::startCapture);
        connect(btnStop, &QPushButton::clicked, this, &LogicAnalyzerWindow::stopCapture);
        connect(cbSampleRate, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbSampleRate](int index){
            emit sampleRateChanged(cbSampleRate->itemData(index).toInt());
        });
        connect(cbTrigCh, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbTrigCh](int index){
            emit triggerChannelChanged(cbTrigCh->itemData(index).toInt());
        });
        connect(cbTrigEdge, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, cbTrigEdge](int index){
            emit triggerEdgeChanged(cbTrigEdge->itemData(index).toInt());
        });
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
    qDebug() << "LogicAnalyzerWindow::showData() - Start. Samples:" << (data.isEmpty() ? 0 : data[0].size());
    if (!chart) return;
    
    QVector<QVector<QPointF>> traces(8);
    double timeStep = 1.0 / sampleRate;
    
    for (int ch = 0; ch < 8; ++ch) {
        if (ch >= data.size()) break;
        const auto &channelData = data[ch];
        traces[ch].reserve(channelData.size() * 2); 
        
        for (int i = 0; i < channelData.size(); ++i) {
            double t = i * timeStep;
            double val = channelData[i] * 0.8 + (ch * 2.0);
            
            if (i > 0) {
                double prevVal = channelData[i-1] * 0.8 + (ch * 2.0);
                traces[ch].append(QPointF(t, prevVal));
            }
            traces[ch].append(QPointF(t, val));
        }
        chart->updateTrace(&traces[ch], ch);
    }
    qDebug() << "LogicAnalyzerWindow::showData() - End";
}
