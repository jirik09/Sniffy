#include "arbgeneratorwindow.h"
#include "ui_arbgeneratorwindow.h"

ArbGeneratorWindow::ArbGeneratorWindow(ArbGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArbGeneratorWindow),
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
    verticalLayout_chart->addWidget(chart);

    QVBoxLayout hovno;

    setting = new ArbGenPanelSettings(verticalLayout_settings,this);

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600,300);
    ui->widget_module->setLayout(verticalLayout_chart);

    generatorChartData = new QVector<QVector<QPointF>>;
    generatorDACData = new QList<QList<int>>;

    connect(setting,&ArbGenPanelSettings::signalChanged,this,&ArbGeneratorWindow::createSignalCallback);
    connect(setting->buttonsGenerate,&WidgetButtons::clicked,this,&ArbGeneratorWindow::runGeneratorCallback);
    connect(setting->buttonSelectFile,&WidgetButtons::clicked,this,&ArbGeneratorWindow::openFileCallback);
}

ArbGeneratorWindow::~ArbGeneratorWindow()
{
    delete ui;
}


void ArbGeneratorWindow::restoreGUIAfterStartup()
{
    setting->restoreGUI();
}


void ArbGeneratorWindow::setSpecification(ArbGeneratorSpec* spec)
{
    this->spec = spec;
    setting->setMaxNumChannels(spec->maxDACChannels);
}

QList<QList<int>> *ArbGeneratorWindow::getGeneratorDACData() const
{
    return generatorDACData;
}

qreal ArbGeneratorWindow::getFrequency(int channel)
{
    return setting->dialFreqCh[channel]->getRealValue();
}

void ArbGeneratorWindow::setProgress(int percent)
{
    setGenerateButton("Uploading "+ QString::number(percent)+ "%",COLOR_ORANGE);
}

void ArbGeneratorWindow::setGeneratorRuning()
{
    setGenerateButton("Stop",COLOR_GREEN);
    isGenerating = true;
}

void ArbGeneratorWindow::setGeneratorStopped()
{
    setGenerateButton("Start",COLOR_BLUE);
    isGenerating = false;
    createSignalCallback();
}

void ArbGeneratorWindow::setFrequencyLabels(int channel, qreal freq)
{
    setting->setFreqLabel(LabelFormator::formatOutout(freq,"Hz",4),channel);
}

void ArbGeneratorWindow::setGenerateButton(QString text, QString color)
{
    setting->buttonsGenerate->setText(text);
    setting->buttonsGenerate->setColor(QString::fromUtf8("background-color:")+color,0);
}

void ArbGeneratorWindow::runGeneratorCallback()
{
    if(setting->buttonsGenerate->getText(0) == "Start"){
        emit runGenerator();
        setGenerateButton("Uploading",COLOR_ORANGE);
    }else{
        emit stopGenerator();
        setGeneratorStopped();
    }
}

void ArbGeneratorWindow::createSignalCallback()
{
    QList<qreal> *freq = new QList<qreal>;
    QList<SignalShape> *shape = new QList<SignalShape>;
    int length;
    int numChannelsUsed;
    QList<int> DACData;
    QList<qreal> signalData;
    QVector<QPointF> chartSignal;
    qreal x(0);
    qreal y(0);
    qreal maxX;

    // **************** build the data for calculation and get the signal lengths. ****************
    if(!isGenerating){
        for (int i = 0;i <MAX_ARB_CHANNELS_NUM ;i++ ) {
            if(setting->channelEnabled[i]){
                freq->append(setting->dialFreqCh[i]->getRealValue());
                shape->append(setting->signalShape[i]);
            }
        }
        numChannelsUsed = freq->length();

        generatorChartData->clear();
        generatorDACData->clear();
        maxX = -10000;

        for(int i = 0;i<numChannelsUsed;i++){
            chartSignal.clear();

            // **************** generate signal based on inputs ****************
            length = SignalCreator::calculateSignalLength(setting->memorySet,setting->customLength, spec->generatorBufferSize/numChannelsUsed,freq->at(i),spec->maxSamplingRate,spec->periphClockFrequency);

            qreal div = (qreal)(spec->periphClockFrequency)/freq->at(i)/length;
            if(div<spec->periphClockFrequency/spec->maxSamplingRate){
                div = spec->periphClockFrequency/spec->maxSamplingRate;
            }
            qreal realfreq = spec->periphClockFrequency/(qreal)((int)(div))/length;
            setting->setLabels(LabelFormator::formatOutout(realfreq,"Hz",3),QString::number(length),i);
            signalData = SignalCreator::createSignal(setting->signalShape[i], length, setting->dialAmplitudeCh[i]->getRealValue(), setting->dialOffsetCh[i]->getRealValue(), setting->dialDutyCh[i]->getRealValue(), setting->dialPhaseCh[i]->getRealValue(),spec->rangeMin,spec->rangeMax);
            //If it is arb data then handle here and fix the datalength

            // **************** calculate data for charts ****************
            for (int j=0 ;j<length ;j++ ) {
                y = signalData[j];
                x = (qreal)(div)/spec->periphClockFrequency*j;// /1/realfreq*j;
                chartSignal.append(QPointF(x,y));
                x = (qreal)(div)/spec->periphClockFrequency*(j+1);// /1/realfreq*j;
                chartSignal.append(QPointF(x,y));
            }
            x = (qreal)(div)/spec->periphClockFrequency*length;
            chartSignal.append(QPointF(x,chartSignal[0].y()));
            if(maxX<x){
                maxX=x;
            }
            generatorChartData->append(chartSignal);

            //**************** calculate data for MCU ****************
            DACData.clear();
            for (int j=0 ;j<length ;j++) {
                int tmpDAC = ((pow(2,12)-1)*(qreal)(signalData[j]-spec->rangeMin))/(spec->rangeMax-spec->rangeMin);
                DACData.append(tmpDAC);
            }
            generatorDACData->append(DACData);
        }

        // **************** plot the data ****************
        chart->clearAll();
        for(int i = 0;i<numChannelsUsed;i++){
            chart->updateTrace(&((*generatorChartData)[i]), i);
        }
        chart->setRange(0,maxX,spec->rangeMin,spec->rangeMax);
    }

    if(isGenerating){
        emit updateFrequency();
    }
}

void ArbGeneratorWindow::openFileCallback()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,"Select output file","","Text files (*.csv *.txt)");
    //config->datalogFileName = fileName;
    qDebug() << fileName;
    if(fileName.length()>=25){
        fileName = fileName.left(8)+" ... "+fileName.right(12);
    }
    if(fileName.length()>3){
     //   labelFile->setValue(fileName);
      //  buttonStartLog->enableAll(true);
    }
}

