#include "arbgensweepcontroller.h"

ArbGenSweepController::ArbGenSweepController(QObject *parent):
    QObject(parent)
{
   timer = new QTimer(this);
   connect(timer, &QTimer::timeout, this, &ArbGenSweepController::timerCallback);
}

void ArbGenSweepController::setParameters(qreal minFreq, qreal maxFreq, qreal totalTime)
{
    actualIndex = 0;
    this->minFreq = minFreq;
    this->maxFreq = maxFreq;
    this->totalTime = totalTime;

    numSamples = (totalTime+0.001)/SWEEP_TIME_STEP;
    sampleTime = totalTime/numSamples;

    freqSamples.clear();
    for(int i = 0;i<numSamples;i++){
        qreal tmpFreq =  minFreq + (qreal)(i)/(numSamples-1) * (maxFreq - minFreq);
        freqSamples.append(tmpFreq);
    }

    timer->setInterval(sampleTime*1000);
    timer->setTimerType(Qt::PreciseTimer);
}

void ArbGenSweepController::startTimer()
{
    if(!isRunning)timer->start();
    isRunning = true;
}

void ArbGenSweepController::stopTimer()
{
    if (isRunning)timer->stop();
    isRunning = false;
}

void ArbGenSweepController::timerCallback()
{
    emit updateSweepFrequency(freqSamples.at(actualIndex));
    QDateTime date = QDateTime::currentDateTime();
    qDebug() <<date.time()<<freqSamples.at(actualIndex) << "Hz";
    actualIndex = (actualIndex+1)%numSamples;

}
