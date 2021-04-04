#include "arbgeneratorwindow.h"
#include "ui_arbgeneratorwindow.h"

ArbGeneratorWindow::ArbGeneratorWindow(ArbGeneratorConfig *config, bool isPWMbased, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArbGeneratorWindow),
    config(config),
    isPWMbased(isPWMbased)
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
    chart->setGraphColor(QCOLOR_GREY);
    verticalLayout_chart->addWidget(chart);

    PWMchart = new widgetChart(widget_chart, 4);
    PWMchart->setRange(0, 1, 0, 1);
    PWMchart->setGraphColor(QCOLOR_GREY);
    verticalLayout_chart->addWidget(PWMchart);
    if(!isPWMbased){
        PWMchart->hide();
    }

    setting = new ArbGenPanelSettings(verticalLayout_settings,isPWMbased,this);
    fileLoader = new ArbGeneratorFileLoader();

    ui->widget_settings->setLayout(verticalLayout_settings);
    ui->widget_module->resize(600,300);
    ui->widget_module->setLayout(verticalLayout_chart);

    generatorChartData = new QVector<QVector<QPointF>>;
    generatorPWMChartData = new QVector<QVector<QPointF>>;
    generatorDACData = new QList<QList<int>>;

    sweepController = new ArbGenSweepController();

    connect(setting,&ArbGenPanelSettings::signalChanged,this,&ArbGeneratorWindow::createSignalCallback);
    connect(setting,&ArbGenPanelSettings::syncRequest,this,&ArbGeneratorWindow::syncRequestCallback);
    connect(setting->buttonsGenerate,&WidgetButtons::clicked,this,&ArbGeneratorWindow::runGeneratorCallback);
    connect(setting->buttonSelectFile,&WidgetButtons::clicked,this,&ArbGeneratorWindow::openFileCallback);
    connect(sweepController, &ArbGenSweepController::updateSweepFrequency, this,&ArbGeneratorWindow::sweepTimerCallback);
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
    fileLoader->setSignalRangeParameters(spec->DACResolution,spec->rangeMin,spec->rangeMax);
}

QList<QList<int>> *ArbGeneratorWindow::getGeneratorDACData() const
{
    return generatorDACData;
}

qreal ArbGeneratorWindow::getFrequency(int channel)
{
    return setting->dialFreqCh[channel]->getRealValue();
}

qreal ArbGeneratorWindow::getPWMFrequency(int channel)
{
    return setting->dialPWMFreqCh[channel]->getRealValue();
}

void ArbGeneratorWindow::setProgress(int percent)
{
    setGenerateButton("Uploading "+ QString::number(percent)+ "%",COLOR_ORANGE);
}

void ArbGeneratorWindow::setGeneratorRuning()
{
    setGenerateButton("Stop",COLOR_GREEN);
    isGenerating = true;
    if(setting->isSweepEnabled){
        sweepController->startTimer();
    }
}

void ArbGeneratorWindow::setGeneratorStopped()
{
    setGenerateButton("Start",COLOR_BLUE);
    isGenerating = false;
    createSignalCallback();
    if(setting->isSweepEnabled){
        sweepController->stopTimer();
    }
}

void ArbGeneratorWindow::setFrequencyLabels(int channel, qreal freq)
{
    setting->setFreqLabel(LabelFormator::formatOutout(freq,"Hz",5),channel);
}

void ArbGeneratorWindow::setPWMFrequencyLabels(int channel, qreal freq)
{
    setting->setPWMFreqLabel(LabelFormator::formatOutout(freq,"Hz",4),channel);
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
        setting->disableGUI();
    }else{
        emit stopGenerator();
        setGeneratorStopped();
        setting->enableGUI();
    }
}

void ArbGeneratorWindow::createSignalCallback()
{
    int length;
    int numChannelsUsed;
    QList<int> MCUData;
    QVector<qreal> signalData;
    QVector<QPointF> chartSignal;
    qreal x(0);
    qreal y(0);
    int PWMres;
    int tmpDAC;
    qreal maxX = -10000;
    qreal maxRes = -10000;

    QList<qreal> *freq = new QList<qreal>;
    QList<SignalShape> *shape = new QList<SignalShape>;
    for (int i = 0;i <MAX_ARB_CHANNELS_NUM ;i++ ) {
        if(setting->channelEnabled[i]){
            freq->append(setting->dialFreqCh[i]->getRealValue());
            shape->append(setting->signalShape[i]);
        }
    }
    numChannelsUsed = freq->length();

    // **************** build the data for calculation and get the signal lengths. ****************
    if(!isGenerating){
        generatorChartData->clear();
        generatorDACData->clear();
        generatorPWMChartData->clear();


        for(int i = 0;i<numChannelsUsed;i++){
            if(isPWMbased){
                PWMres = spec->periphPWMClockFrequency/(int)(setting->dialPWMFreqCh[i]->getRealValue());
                while(PWMres>65535)
                    PWMres = (PWMres+1)/2-1;
                setting->setPWMResolutionLabel(LabelFormator::formatOutout(log2(PWMres),"Bits",1),i);
            }

            chartSignal.clear();
            MCUData.clear();

            // **************** generate signal based on inputs ****************
            if(shape->at(i) == SignalShape::ARB){
                length = fileLoader->getSignalLength(i);
            }else{
                length = SignalCreator::calculateSignalLength(setting->memorySet,setting->customLength, spec->generatorBufferSize/numChannelsUsed,freq->at(i),spec->maxSamplingRate,spec->periphClockFrequency);
            }

            qreal div = (qreal)(spec->periphClockFrequency)/freq->at(i)/length;
            if(div<spec->periphClockFrequency/spec->maxSamplingRate){
                div = spec->periphClockFrequency/spec->maxSamplingRate;
            }
            qreal realfreq = spec->periphClockFrequency/(qreal)((int)(div))/length;
            setting->setLabels(LabelFormator::formatOutout(realfreq,"Hz",4),QString::number(length),i);

            if(shape->at(i) == SignalShape::ARB){
                signalData = fileLoader->getSignal(i);
                if(setting->dialPhaseCh[i]->getRealValue()!=0){
                    signalData = SignalCreator::shiftPhase(signalData,setting->dialPhaseCh[i]->getRealValue());
                }
                for (int j=0 ;j<length ;j++ ) {
                    y = signalData[j];
                    y = (y -fileLoader->getOffset(i)) *(setting->dialAmplitudeCh[i]->getRealValue()/fileLoader->getAmplitude(i))+fileLoader->getOffset(i); //gain
                    y = y + (setting->dialOffsetCh[i]->getRealValue()-fileLoader->getOffset(i)); //offset
                    y = fmax(fmin(y,spec->rangeMax),spec->rangeMin); //min max range

                    x = (qreal)(div)/spec->periphClockFrequency*j;// /1/realfreq*j;
                    chartSignal.append(QPointF(x,y));
                    x = (qreal)(div)/spec->periphClockFrequency*(j+1);// /1/realfreq*j;
                    chartSignal.append(QPointF(x,y));

                    if(isPWMbased){
                        tmpDAC = PWMres-PWMres*((qreal)(y-spec->rangeMin))/(spec->rangeMax-spec->rangeMin);
                    }else{
                        tmpDAC = ((pow(2,spec->DACResolution)-1)*(qreal)(y-spec->rangeMin))/(spec->rangeMax-spec->rangeMin);
                    }


                    MCUData.append(tmpDAC);
                }
                x = (qreal)(div)/spec->periphClockFrequency*length;
                chartSignal.append(QPointF(x,chartSignal[0].y()));
                if(maxX<x)maxX=x;

                generatorChartData->append(chartSignal);
                generatorDACData->append(MCUData);

            }else{ //Other signals except arbitrary
                signalData = SignalCreator::createSignal(setting->signalShape[i], length, setting->dialAmplitudeCh[i]->getRealValue(), setting->dialOffsetCh[i]->getRealValue(), setting->dialDutyCh[i]->getRealValue(), setting->dialPhaseCh[i]->getRealValue(),spec->rangeMin,spec->rangeMax);
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
                if(maxX<x) maxX=x;

                generatorChartData->append(chartSignal);

                //**************** calculate data for MCU ****************
                MCUData.clear();
                for (int j=0 ;j<length ;j++) {
                    if(isPWMbased){
                        tmpDAC = PWMres-PWMres*(qreal)(signalData[j]-spec->rangeMin)/(spec->rangeMax-spec->rangeMin);
                    }else{
                        tmpDAC = ((pow(2,spec->DACResolution)-1)*(qreal)(signalData[j]-spec->rangeMin))/(spec->rangeMax-spec->rangeMin);
                    }
                    MCUData.append(tmpDAC);
                }
                generatorDACData->append(MCUData);
            }

            //*********** calculate signal for PWM chart *******
            if(isPWMbased){
                if(PWMres>maxRes)maxRes = PWMres;
                chartSignal.clear();
                int increment = ceil((qreal)(length)/ PWM_CHART_SAMPLES);
                for (int j=0 ;j<length ;j+=increment) {
                    x = 0;
                    y = spec->Vcc+i;
                    chartSignal.append(QPointF(x,y));
                    x = PWMres-MCUData[j];
                    chartSignal.append(QPointF(x,y));
                    y = i;
                    chartSignal.append(QPointF(x,y));
                    x = PWMres;
                    chartSignal.append(QPointF(x,y));
                    j+=increment;
                    if(j>=length)j = length-1;
                    x = PWMres-MCUData[j];
                    chartSignal.append(QPointF(x,y));
                    y = spec->Vcc+i;
                    chartSignal.append(QPointF(x,y));
                    x = 0;
                    chartSignal.append(QPointF(x,y));
                }
                generatorPWMChartData->append(chartSignal);
            }
        }


    }else{ //generating but charts need to be updated anyway
        QVector<QPointF> tmpVect;

        for (int i = 0;i<generatorChartData->length() ;i++ ) {
            tmpVect.clear();

            qreal div = (qreal)(spec->periphClockFrequency) / freq->at(i) / generatorChartData->at(i).length();
            if(div<spec->periphClockFrequency/spec->maxSamplingRate){
                div = spec->periphClockFrequency/spec->maxSamplingRate;
            }

            for(int j = 0;j<generatorChartData->at(i).length();j=j+2){
                y = generatorChartData->at(i).at(j).y();
                x = (qreal)(div) / spec->periphClockFrequency*j/2;// /1/realfreq*j;
                tmpVect.append(QPointF(x,y));
                x = (qreal)(div) / spec->periphClockFrequency*(j/2+1);// /1/realfreq*j;
                tmpVect.append(QPointF(x,y));
            }
            if(maxX<x) maxX=x;
            generatorChartData->replace(i,tmpVect);
        }
    }


    // **************** plot the data ****************
    chart->clearAll();
    for(int i = 0;i<numChannelsUsed;i++){
        chart->updateTrace(&((*generatorChartData)[i]), i);
    }
    chart->setRange(0,maxX,spec->rangeMin,spec->rangeMax);


    // **************** plot the PWM data ****************
    if(isPWMbased){
        PWMchart->clearAll();
        for(int i = 0;i<numChannelsUsed;i++){
            PWMchart->updateTrace(&((*generatorPWMChartData)[i]), i);
            PWMchart->setHorizontalMarker(i,i);
        }
        PWMchart->setRange(0,maxRes,spec->Vcc*(-0.1),spec->Vcc*1.1+(numChannelsUsed-1));
    }


    if(setting->isSweepEnabled){
        sweepController->setParameters(setting->dialFreqSweepMin->getRealValue(),setting->dialFreqSweepMax->getRealValue(),setting->dialFreqSweepTime->getRealValue());
    }

    if(isGenerating){
        if(setting->isSweepEnabled){
            sweepController->startTimer();
        }else{
            sweepController->stopTimer();
        }
        emit updateFrequency();
    }
}

void ArbGeneratorWindow::openFileCallback()
{
    QString fileName;
    int tmp = 0;
    fileName = QFileDialog::getOpenFileName(this,"Select input file","","Text files (*.csv *.txt)");
    tmp = fileLoader->parseFile(fileName);
    for (int i = 0;i<fileLoader->getNumChannels();i++){
        if(fileLoader->isSamplerateDefined()){
            setting->dialFreqCh[i]->setRealValue(fileLoader->getSamplerateFrequency()/fileLoader->getSignalLength(i));
        }
        setting->dialAmplitudeCh[i]->setRealValue(fileLoader->getAmplitude(i));
        setting->dialOffsetCh[i]->setRealValue(fileLoader->getOffset(i));
        setting->dialPhaseCh[i]->setRealValue(0,true);
    }
    if(tmp>0){
        if(fileLoader->getParsingErrors()==0){
            setting->labelArbFileInfo->setName("File parsed:");
            setting->labelArbFileInfo->setValue(fileLoader->getInfoString());
        }else{
            setting->labelArbFileInfo->setName("File parsed with errors:");
            setting->labelArbFileInfo->setValue(QString::number(fileLoader->getParsingErrors()));
        }

    }else if(tmp==-1){
        setting->labelArbFileInfo->setValue("Could not open the file");
        setting->labelArbFileInfo->setName("Error");
    }else if(tmp==-2){
        setting->labelArbFileInfo->setValue("No valid separator (, or ;)");
        setting->labelArbFileInfo->setName("Error");
    }else if(tmp==-4){
        setting->labelArbFileInfo->setValue("File name is incorrect");
        setting->labelArbFileInfo->setName("Error");
    }

    createSignalCallback();
}

void ArbGeneratorWindow::sweepTimerCallback(qreal frequency)
{
    setting->dialFreqCh[0]->setRealValue(frequency,true);
    for (int i = 1;i<MAX_ARB_CHANNELS_NUM ; i++) {
        if(setting->swSyncWithCH1[i]->isCheckedRight()){
            setting->dialFreqCh[i]->setRealValue(frequency,true);
        }
    }
    emit updateFrequency();
}

void ArbGeneratorWindow::syncRequestCallback()
{
    if(isGenerating){
        emit restartGenerating();
    }
}
