#include "fftengine.h"

FFTengine::FFTengine(QObject *parent) : QThread(parent)
{
    restart = false;
    abort = false;
}

FFTengine::~FFTengine()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void FFTengine::calculate(QVector<QPointF> data,FFTWindow window,FFTType type, int minNFFT, bool removeDC, qreal samplingFreq)
{
    dataIn = data;
    //dataIn.resize(2048);
    this->window = window;
    this->minNFFT = minNFFT;
    this->type = type;
    this->removeDC = removeDC;
    this->samplingFreq = samplingFreq;
    QMutexLocker locker(&mutex);

    if (!isRunning()) {
        start(NormalPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

QVector<QPointF> FFTengine::getProcessedData()
{
    return outputData;
}

qreal FFTengine::getMaxFrequency()
{
    return maxFrequency;
}

void FFTengine::run()
{
    QVector<std::complex<float>> tmpOutput;
    QVector<std::complex<float>> tmpData;
    QVector<QPointF> tmpIn;
    forever {
        mutex.lock();
        tmpIn = dataIn;
        tmpOutput.clear();
        tmpData.clear();
        mutex.unlock();

        if (removeDC) {
            qreal dc = 0;
            for (int i = 0; i < tmpIn.length(); i++)
                dc += tmpIn.at(i).y();
            dc = dc / tmpIn.length();

            for (int i = 0; i < tmpIn.length(); i++)
                tmpIn[i].setY(tmpIn.at(i).y()-dc);
        }

        for (int i = 0; i < tmpIn.length(); i++)
            tmpData.append(std::complex<float>(tmpIn.at(i).y(), 0));

        tmpOutput = calculateSpectrum(tmpData,window,minNFFT);
        outputData = processOutput(tmpOutput,type);

        if (abort)
            return;

        //if (!restart){
        outputSpectrum = tmpOutput;
        emit fftCalculated();
        // }
        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}

QVector<std::complex<float>> FFTengine::calculateSpectrum(QVector<std::complex<float>> data, FFTWindow window, int minNFFT) {
    if (window == FFTWindow::hamming) {
        resizeHamming(data.length());
        for (int i = 0; i < data.size(); i++)
            data[i] *= hamming.at(i);
    } else if (window == FFTWindow::hann) {
        resizeHann(data.length());
        for (int i = 0; i < data.size(); i++)
            data[i] *= hann.at(i);
    } else if (window == FFTWindow::blackman) {
        resizeBlackman(data.length());
        for (int i = 0; i < data.size(); i++)
            data[i] *= blackman.at(i);
    }

    int nfft = nextPow2(data.size());
    if (nfft < minNFFT)
        nfft = minNFFT;
    data.resize(nfft);

    return fft(data);
}

QVector<QPointF> FFTengine::processOutput(QVector<std::complex<float>> data, FFTType type) {
    QVector<QPointF> result;
    int nfft = data.length();
    float freq = 0;
    double freqStep = samplingFreq / nfft;
    for (int i = 0; i <= nfft / 2; i++) {
        freq = i * freqStep;
        if (type == FFTType::periodogram) {
            // |x|^2 calculatd as  x * complex conjugate x;
            float absSquared = (data.at(i) * std::complex<float>(data.at(i).real(), - data.at(i).imag())).real();
            result.append(QPointF(freq, 10 * log10(absSquared / nfft)));
        } else
            result.append(QPointF(freq, std::abs(data.at(i))));
    }
    maxFrequency = freq;
    return result;
}

QVector<std::complex<float>> FFTengine::fft(QVector<std::complex<float>> x) {
    int N = x.size(); // Velikost x musí být mocnina 2.

    if (N == 1)
        return x; // Konec rekurze

    // exp(2*Pi*i/N)
    auto Wn = std::exp(std::complex<float>(0, 2.0 * M_PI / (float)N));

    QVector<std::complex<float>> Pe, Po; // Sudé a liché koeficienty
    for (int n = 1; n < N; n += 2) {
        Pe.append(x.at(n - 1)); // Sudé
        Po.append(x.at(n));     // Liché
    }

    QVector<std::complex<float>> Xe = fft(Pe), Xo = fft(Po); // Rekurze

    QVector<std::complex<float>> X;
    X.resize(N);
    for (int k = 0; k < N / 2; k++) {
        std::complex<float> Wi = std::pow(Wn, k);
        X[k] = Xe[k] + Wi * Xo[k];
        X[k + N / 2] = Xe[k] - Wi * Xo[k];
    }
    return X;
}

void FFTengine::resizeHamming(int length) {
    if (hamming.size() != length) {
        hamming.resize(length);
        for (int n = 0; n < length; n++)
            hamming[n] = 0.54 - 0.46 * cos(2 * M_PI * n / length);
    }
}

void FFTengine::resizeHann(int length) {
    if (hann.size() != length) {
        hann.resize(length);
        for (int n = 0; n < length; n++)
            hann[n] = 0.5 * (1 - cos(2 * M_PI * n / length));
    }
}

void FFTengine::resizeBlackman(int length) {
    if (blackman.size() != length) {
        blackman.resize(length);
        for (int n = 0; n < length; n++)
            blackman[n] = 0.42 - 0.5 * cos(2 * M_PI * n / (length - 1)) + 0.08 * cos(4 * M_PI * n / (length - 1));
    }
}

int FFTengine::nextPow2(int number) {
    for (int i = 1;; i++)
        if (pow(2, i) >= number)
            return (pow(2, i));
}


