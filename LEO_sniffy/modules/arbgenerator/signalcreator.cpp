#include "signalcreator.h"

SignalCreator::SignalCreator(QObject *parent) : QObject(parent)
{

}

QList<qreal> SignalCreator::createSignal(SignalShape shape, int numSamples, qreal amplitude, qreal offset, qreal duty, qreal phase, qreal rangeMin, qreal rangeMax)
{
    QList<qreal> signal;
    qreal shift = (int)(phase / 360 * numSamples);
    switch (shape) {
    case SignalShape::SINE:
        for(int i = 0;i<numSamples;i++){
            qreal tmp = sin(2*M_PI*i/numSamples+phase/180*M_PI)*amplitude+offset;
            if(tmp<rangeMin){
                tmp = rangeMin;
            }
            if(tmp >rangeMax){
                tmp = rangeMax;
            }

            signal.append(tmp);
        }
        break;
    case SignalShape::SAW:
        for (int j = 0; j < numSamples; j++){
            if (j > numSamples * duty / 100){
                signal.append(offset - amplitude + amplitude * 2 - amplitude * 2 / (numSamples - (duty / 100 * numSamples)) * (j - (numSamples * duty / 100)));
            }
            else{
                signal.append(offset - amplitude + amplitude * 2 / (duty / 100 * numSamples) * j);
                //tmpSignal[(j + shift) % tmpSignal.Length] =
            }
        }
        break;
    case SignalShape::RECT:
        for (int j = 0; j < numSamples; j++){
            if (j < duty / 100 * numSamples){
                signal.append(offset - amplitude);
            }
            else{
                signal.append(offset + amplitude);
            }
        }
        break;
    case SignalShape::ARB:
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
    int bestDiv = 0;
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
                    bestDiv = div;
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
            qDebug () << bestDiv;
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


        /* int divA = tclk / device.genCfg.maxSamplingFrequency;
        int divB = (int)(tclk / tmp_freq_ch1 / signal_leng_ch1);
        divider_ch1 = divA > divB ? divA : divB;

        divA = tclk / device.genCfg.maxSamplingFrequency;
        divB = (int)(tclk / tmp_freq_ch2 / signal_leng_ch2);
        divider_ch2 = divA > divB ? divA : divB;
        break;*/


    }

    return *outputLeng;
}




/*        public void calculate_signal_lengths()
        {
            int tclk = device.systemCfg.PeriphClock;
            //estimate length and divider

             * *Best frequency fit*
             * 1 - Estimate minimal possible divider (sampling frequency or max length of signal)
             * 2 - Calculate desired signal length for current divider
             * 3 - Round signal length and calculate error
             * 4 - Increment divider and calculate error again while error small enough or signal length too small

            if (bestFreqFit)
            {

                double tmp_freq = checkBox_khz_ch1.Checked ? freq_ch1 * 1000 : freq_ch1;

                int divA = tclk / device.genCfg.maxSamplingFrequency;
                int divB = (int)(tclk / tmp_freq / (device.genCfg.BufferLength / 2) * actual_channels);
                int div = divA > divB ? divA : divB;
                double error;
                double minimalError;
                int bestDiv = 0;
                int bestLeng = 0;
                double tmpSigLeng = 0; ;

                for (int i = 1; i <= actual_channels; i++)
                {
                    if ((i == 1 && signalType_ch1 == SIGNAL_TYPE.ARB) || (i == 2 && signalType_ch2 == SIGNAL_TYPE.ARB))
                    {
                        divider_ch1 = (int)(tclk / freq_ch1 / signal_leng_ch1);
                        divider_ch2 = (int)(tclk / freq_ch2 / signal_leng_ch2);
                        continue;
                    }

                    error = double.MaxValue;
                    minimalError = double.MaxValue;
                    if (i == 2)
                    {
                        tmp_freq = checkBox_khz_ch2.Checked ? freq_ch2 * 1000 : freq_ch2;
                        divB = (int)Math.Round((double)tclk / tmp_freq / (device.genCfg.BufferLength / 2) * actual_channels);
                        div = divA > divB ? divA : divB;
                    }

                    int iter = 0;
                    while (error > 0)
                    {
                        tmpSigLeng = tclk / tmp_freq / div;
                        error = Math.Abs(tmp_freq - (double)(tclk) / (div * (int)Math.Round(tmpSigLeng)));

                        if (tmpSigLeng - 0.0001 > (device.genCfg.BufferLength / 2 / actual_channels))
                        {
                            div++;
                            iter++;
                            continue;
                        }
                        if (error < minimalError)
                        {
                            bestDiv = div;
                            bestLeng = (int)Math.Round(tmpSigLeng);
                            minimalError = error;
                        }

                        if (error < 0.01)
                        {
                            break;
                        }

                        if (tmpSigLeng < (device.genCfg.BufferLength / 2 / actual_channels / 4) && iter > 5)
                        {
                            break;
                        }
                        div++;
                        iter++;
                    }

                    if (i == 1)
                    {
                        signal_leng_ch1 = bestLeng;
                        divider_ch1 = bestDiv;
                    }
                    else
                    {
                        signal_leng_ch2 = bestLeng;
                        divider_ch2 = bestDiv;
                    }
                }
            }
            else
            { //isn't best frequency fit
                double tmp_freq_ch1 = checkBox_khz_ch1.Checked ? freq_ch1 * 1000 : freq_ch1;
                double tmp_freq_ch2 = checkBox_khz_ch2.Checked ? freq_ch2 * 1000 : freq_ch2;
                if (customLeng)  //custom length
                {
                    if (signalType_ch1 != SIGNAL_TYPE.ARB)
                    {
                        //int.TryParse(toolStripTextBox_signal_leng.Text, out signal_leng_ch1);
                        signal_leng_ch1 = signal_leng;
                    }

                    if (signalType_ch2 != SIGNAL_TYPE.ARB)
                    {
                        //int.TryParse(toolStripTextBox_signal_leng.Text, out signal_leng_ch2);
                        signal_leng_ch2 = signal_leng;
                    }

                    int divA = tclk / device.genCfg.maxSamplingFrequency;
                    int divB = (int)(tclk / tmp_freq_ch1 / signal_leng_ch1);
                    divider_ch1 = divA > divB ? divA : divB;

                    divA = tclk / device.genCfg.maxSamplingFrequency;
                    divB = (int)(tclk / tmp_freq_ch2 / signal_leng_ch2);
                    divider_ch2 = divA > divB ? divA : divB;
                }
                else //maximum possible
                {
                    int div = tclk / device.genCfg.maxSamplingFrequency;
                    double leng = (double)tclk / tmp_freq_ch1 / div;
                    if (signalType_ch1 != SIGNAL_TYPE.ARB)
                    {
                        if (leng > device.genCfg.BufferLength / 2 / actual_channels)
                        {
                            signal_leng_ch1 = device.genCfg.BufferLength / 2 / actual_channels;
                            divider_ch1 = (int)(tclk / tmp_freq_ch1 / signal_leng_ch1);
                        }
                        else
                        {
                            signal_leng_ch1 = (int)leng;
                            divider_ch1 = (int)(tclk / tmp_freq_ch1 / signal_leng_ch1);
                        }
                    }
                    else
                    {
                        divider_ch1 = (int)(tclk / freq_ch1 / signal_leng_ch1);
                    }

                    leng = (double)tclk / tmp_freq_ch2 / div;
                    if (signalType_ch2 != SIGNAL_TYPE.ARB)
                    {
                        if (leng > device.genCfg.BufferLength / 2 / actual_channels)
                        {
                            signal_leng_ch2 = device.genCfg.BufferLength / 2 / actual_channels;
                            divider_ch2 = (int)(tclk / tmp_freq_ch2 / signal_leng_ch2);
                        }
                        else
                        {
                            signal_leng_ch2 = (int)leng;
                            divider_ch2 = (int)(tclk / tmp_freq_ch2 / signal_leng_ch2);
                        }
                    }
                    else
                    {
                        divider_ch2 = (int)(tclk / freq_ch2 / signal_leng_ch2);
                    }
                }
            }
        }*/
