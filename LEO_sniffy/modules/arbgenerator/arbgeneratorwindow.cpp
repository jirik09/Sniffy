#include "arbgeneratorwindow.h"
#include "ui_arbgeneratorwindow.h"

ArbGeneratorWindow::ArbGeneratorWindow(ArbGeneratorConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArbGeneratorWindow),
    config(config)
{
    ui->setupUi(this);
    //TODO setup the GUI here

    QWidget *widget_settings = new QWidget(this, Qt::Window);
    QWidget *widget_chart = new QWidget(this, Qt::Window);
    QVBoxLayout *verticalLayout_chart = new QVBoxLayout();
    QVBoxLayout *verticalLayout_settings = new QVBoxLayout();
    widget_settings->setLayout(verticalLayout_settings);
    widget_chart->setLayout(verticalLayout_chart);

    widget_settings->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    verticalLayout_settings->setContentsMargins(4,4,4,4);
    verticalLayout_settings->setSpacing(2);

    widget_chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget_chart->setContentsMargins(0,0,0,0);
    verticalLayout_chart->setContentsMargins(0,0,0,0);
    verticalLayout_chart->setSpacing(0);

    chart = new widgetChart(widget_chart, 4);
    //chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chart->setRange(0, 1, 0, 1);
    verticalLayout_chart->addWidget(chart);

    setting = new ArbGenPanelSettings(verticalLayout_settings,this);


    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->setLayout(verticalLayout_chart);

    generatorChartData = new QVector<QVector<QPointF>>;
    generatorDACData = new QList<QList<int>>;

    connect(setting,&ArbGenPanelSettings::signalChanged,this,&ArbGeneratorWindow::generateSignalCallback);

}

ArbGeneratorWindow::~ArbGeneratorWindow()
{
    delete ui;
}


void ArbGeneratorWindow::restoreGUIAfterStartup()
{
    setting->restoreGUI();
    //TODO GUI is loaded to previous state
    //validate the GUI appereance (colors according to selected channel
    //disabled/enabled buttons accordign to selected function
}


void ArbGeneratorWindow::setSpecification(ArbGeneratorSpec* spec)
{
    this->spec = spec;
    setting->setMaxNumChannels(spec->maxDACChannels);
}

void ArbGeneratorWindow::generateSignalCallback()
{
    QList<qreal> *freq = new QList<qreal>;
    QList<int> lengths;
    QList<int> DACData;
    QList<qreal> signalData;
    QVector<QPointF> chartSignal;
    qreal x(0);
    qreal y(0);
    qreal maxX;


    // **************** build the data for calculation and get th signal lengths. ****************
    for (int i = 0;i <MAX_ARB_CHANNELS_NUM ;i++ ) {
        if(setting->channelEnabled[i]){
            freq->append(setting->dialFreqCh[i]->getRealValue());
        }
    }
    lengths = SignalCreator::calculateSignalLengths(setting->memorySet,setting->customLength, spec->generatorBufferSize,*freq,spec->maxSamplingRate,spec->periphClockFrequency);
    generatorChartData->clear();
    generatorDACData->clear();
    maxX = -10000;

    for(int i = 0;i<lengths.length();i++){
        chartSignal.clear();

        qreal div = (qreal)(spec->periphClockFrequency)/freq->at(i)/lengths.at(i);
        if(div<spec->periphClockFrequency/spec->maxSamplingRate){
            div = spec->periphClockFrequency/spec->maxSamplingRate;
        }

        // actually generate signal based on inputs
        qreal realfreq = spec->periphClockFrequency/(qreal)((int)(div))/lengths.at(i);
        //qDebug() <<"Channel"<<i << "Length" <<lengths.at(i) << "Div" << div << "real Freq" << realfreq;
        setting->setLabels(LabelFormator::formatOutout(realfreq,"Hz",3),QString::number(lengths.at(i)),i);
        signalData = SignalCreator::createSignal(setting->signalShape[i], lengths[i], setting->dialAmplitudeCh[i]->getRealValue(), setting->dialOffsetCh[i]->getRealValue(), setting->dialDutyCh[i]->getRealValue(), setting->dialPhaseCh[i]->getRealValue(),spec->rangeMin,spec->rangeMax);


        // calculate data for charts
        for (int j=0 ;j<lengths[i] ;j++ ) {
            y = signalData[j];
            x = (qreal)(div)/spec->periphClockFrequency*j;// /1/realfreq*j;
            chartSignal.append(QPointF(x,y));
            x = (qreal)(div)/spec->periphClockFrequency*(j+1);// /1/realfreq*j;
            chartSignal.append(QPointF(x,y));
        }
        x = (qreal)(div)/spec->periphClockFrequency*lengths[i];
        chartSignal.append(QPointF(x,chartSignal[0].y()));
        if(maxX<x){
            maxX=x;
        }
        generatorChartData->append(chartSignal);


        //calculate data for MCU
        DACData.clear();
        for (int j=0 ;j<lengths[i] ;j++) {
            int tmpDAC = ((pow(2,12)-1)*(qreal)(signalData[j]-spec->rangeMin))/(spec->rangeMax-spec->rangeMin);
            DACData.append(tmpDAC);
        }
        generatorDACData->append(DACData);
    }



    // **************** plot the data ****************
    chart->clearAll();
    for(int i = 0;i<lengths.length();i++){
        chart->updateTrace(&((*generatorChartData)[i]), i);
    }
    chart->setRange(0,maxX,spec->rangeMin,spec->rangeMax);


}

