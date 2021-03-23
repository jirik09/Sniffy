#include "signalcreator.h"

SignalCreator::SignalCreator(QObject *parent) : QObject(parent)
{
}

QList<qreal> SignalCreator::createSignal(SignalShape shape, int numSamples, qreal amplitude, qreal offset, qreal duty, qreal phase, qreal rangeMin, qreal rangeMax)
{
    QList<qreal> signal;
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

QList<int> SignalCreator::calculateSignalLengths(MemoryLength memSet, int customLength,int generatorBufferSize, QList<qreal> signalFreq, int maxSamplingRate, int periphClockFrequency)
{
    QList<int> *outputLeng = new QList<int>;
    double tmp_freq;
    int actual_channels = signalFreq.length();
    int divA = periphClockFrequency / maxSamplingRate;
    int divB;
    int div;
    double error;
    double minimalError;
    int bestLeng = 0;
    double tmpSigLeng = 0; ;

    switch (memSet) {
    case MemoryLength::BEST_FIT:

        for (int i = 0; i < actual_channels; i++){
            error = DBL_MAX;
            minimalError = DBL_MAX;
            tmp_freq = signalFreq.at(i);
            divB = (int)(periphClockFrequency / tmp_freq / (generatorBufferSize / 2) * actual_channels);
            div = divA > divB ? divA : divB;

            int iter = 0;
            while (error > 0){
                tmpSigLeng = periphClockFrequency / tmp_freq / div;
                if(tmpSigLeng<=2){
                    tmpSigLeng = 2;
                }
                error = abs(tmp_freq - (double)(periphClockFrequency) / (div * (int)(tmpSigLeng)));

                if (tmpSigLeng - 0.0000001 > (generatorBufferSize / 2 / actual_channels)){
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

                if (tmpSigLeng <=2  || (tmpSigLeng < (generatorBufferSize / 2 / actual_channels / 4) && iter > 20)){
                    break;
                }
                div++;
                iter++;
            }
            outputLeng->append(bestLeng);
        }

        break;
    case MemoryLength::LONG:
        for (int i = 0; i < actual_channels; i++)
        {
            int div = periphClockFrequency / maxSamplingRate;
            double leng = (double)periphClockFrequency / signalFreq.at(i) / div;
            if(leng<=2){
                leng = 2;
            }
            if (leng > generatorBufferSize / 2 / actual_channels){
                outputLeng->append(generatorBufferSize / 2 / actual_channels);
            }
            else{
                outputLeng->append((int)(leng));
            }
        }
        break;
    case MemoryLength::CUSTOM:
        for (int i = 0; i < actual_channels; i++)
        {
            double leng = customLength;
            if(leng<=2){
                leng = 2;
            }
            if (leng > generatorBufferSize / 2 / actual_channels){
                outputLeng->append(generatorBufferSize / 2 / actual_channels);
            }
            else{
                outputLeng->append((int)(leng));
            }
        }
        break;
    }
    return *outputLeng;
}

