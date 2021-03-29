#include "signalcreator.h"

SignalCreator::SignalCreator(QObject *parent) : QObject(parent)
{
}

QVector<qreal> SignalCreator::createSignal(SignalShape shape, int numSamples, qreal amplitude, qreal offset, qreal duty, qreal phase, qreal rangeMin, qreal rangeMax)
{
    QVector<qreal> signal;
    qreal tmpPoint;
    int shift = (int)(phase / 360 * numSamples);
    switch (shape) {
    case SignalShape::SINE:
        for(int i = 0;i<numSamples;i++){
            tmpPoint = sin(2*M_PI*i/numSamples+phase/180*M_PI)*amplitude+offset;

            if(tmpPoint<rangeMin){
                tmpPoint = rangeMin;
            }
            if(tmpPoint >rangeMax){
                tmpPoint = rangeMax;
            }
            signal.append(tmpPoint);
        }
        break;
    case SignalShape::SAW:
        for (int j = 0; j < numSamples; j++){

            if ((j+shift)%numSamples >= numSamples * duty / 100.0){
                tmpPoint = offset - amplitude + amplitude * 2 - amplitude * 2 / (numSamples - (duty / 100.0 * numSamples)) * ((j+shift)%numSamples - (numSamples * duty / 100.0));
            }
            else{
                tmpPoint = offset - amplitude + amplitude * 2 / (duty / 100.0 * numSamples) * ((j+shift)%numSamples);
            }

            if(tmpPoint<rangeMin){
                tmpPoint = rangeMin;
            }
            if(tmpPoint >rangeMax){
                tmpPoint = rangeMax;
            }
            signal.append(tmpPoint);
        }
        break;
    case SignalShape::RECT:
        for (int j = 0; j < numSamples; j++){
            if ((j+shift)%numSamples > (duty) / 100 * numSamples){
                tmpPoint = offset - amplitude;
            }
            else{
                tmpPoint = offset + amplitude;
            }

            if(tmpPoint<rangeMin){
                tmpPoint = rangeMin;
            }
            if(tmpPoint >rangeMax){
                tmpPoint = rangeMax;
            }
            signal.append(tmpPoint);
        }
        break;
    case SignalShape::ARB:
        //Do nothing
        break;
    }

    return signal;
}

int SignalCreator::calculateSignalLength(MemoryLength memSet, int customLength,int generatorBufferSize, qreal signalFreq, int maxSamplingRate, int periphClockFrequency)
{
    int outputLeng;
    int divA = periphClockFrequency / maxSamplingRate;
    int divB;
    int div;
    double error;
    double minimalError;
    int bestLeng = 0;
    double tmpSigLeng = 0;
    int iter = 0;
    double leng;

    switch (memSet) {
    case MemoryLength::BEST_FIT:
        error = DBL_MAX;
        minimalError = DBL_MAX;
        divB = (int)(periphClockFrequency / signalFreq / (generatorBufferSize / 2));
        div = divA > divB ? divA : divB;

        iter = 0;
        while (error > 0){
            tmpSigLeng = periphClockFrequency / signalFreq / div;
            if(tmpSigLeng<=2){
                tmpSigLeng = 2;
            }
            error = abs(signalFreq - (double)(periphClockFrequency) / (div * (int)(tmpSigLeng)));

            if (tmpSigLeng - 0.0000001 > (generatorBufferSize / 2)){
                div++;
                iter++;
                continue;
            }
            if (error < minimalError){
                bestLeng = (int)(tmpSigLeng);
                minimalError = error;
            }
            if (error < 0.01){
                break;
            }

            if (tmpSigLeng <=2  || (tmpSigLeng < (generatorBufferSize / 2 / 4) && iter > 20)){
                break;
            }
            div++;
            iter++;
        }
        outputLeng = bestLeng;
        break;

    case MemoryLength::LONG:
        div = periphClockFrequency / maxSamplingRate;
        leng = (double)periphClockFrequency / signalFreq / div;
        if(leng<=2){
            leng = 2;
        }
        if (leng > generatorBufferSize / 2){
            outputLeng = generatorBufferSize / 2;
        }
        else{
            outputLeng = leng;
        }

        break;

    case MemoryLength::CUSTOM:
        double leng = customLength;
        if(leng<=2){
            leng = 2;
        }
        if (leng > generatorBufferSize / 2){
            outputLeng = generatorBufferSize / 2;
        }
        else{
            outputLeng = leng;
        }
        break;
    }

    return outputLeng;
}

QVector<qreal> SignalCreator::shiftPhase(QVector<qreal> in, qreal phase)
{
    int length = in.length();
    int shift = (qreal)(length)*phase/360;
    QVector<qreal> tmpData;

    for(int i = 0;i<length;i++){
        tmpData.append(in.at((i+shift)%length));
    }
    return tmpData;
}

