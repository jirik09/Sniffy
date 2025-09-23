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

// Contract: Thread-safe entry. Copies (moves) input data under mutex, signals fftCalculated() after processing.
// No exceptions expected; errors would manifest only via invalid sizes (guarded upstream).
void FFTengine::calculate(QVector<QPointF> data, FFTWindow window, FFTType type, int minNFFT, bool removeDC, qreal samplingFreq)
{
    // Lock first to ensure no race with the worker thread reading shared members.
    QMutexLocker locker(&mutex);
    dataIn = std::move(data);
    this->window = window;
    this->lengthNFFT = minNFFT;
    this->type = type;
    this->removeDC = removeDC;
    this->samplingFreq = samplingFreq;

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
        {
            QMutexLocker locker(&mutex);
            tmpIn = dataIn; // snapshot input under lock
            // flags handled after processing
        }
        tmpOutput.clear();
        tmpData.clear();

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

        tmpOutput = calculateSpectrum(tmpData,window,lengthNFFT);
        outputData = processOutput(tmpOutput,type);

        if (abort)
            return;

        //if (!restart){
        outputSpectrum = tmpOutput;
        emit fftCalculated();
        // }
        {
            QMutexLocker locker(&mutex);
            if (!restart)
                condition.wait(&mutex);
            restart = false;
        }
    }
}

QVector<std::complex<float>> FFTengine::calculateSpectrum(QVector<std::complex<float>> data, FFTWindow window, int minNFFT) {
    if(data.length()>minNFFT){
        data.resize(minNFFT);
    }
    // Normalization multipliers (document origin or TODO to derive analytically):
    static constexpr float HAMMING_NORM   = 2.44f;
    static constexpr float HANN_NORM      = 2.67f;
    static constexpr float BLACKMAN_NORM  = 3.125f;
    static constexpr float FLATTOP_NORM   = 1.33f;
    // Rectangular window normalization intentionally left at 1.0f (unity pass-through)

    const int N = data.size();
    switch(window){
    case FFTWindow::hamming: {
        resizeHamming(N);
        for(int i=0;i<N;++i) data[i] *= hamming.at(i)*HAMMING_NORM;
        break;
    }
    case FFTWindow::hann: {
        resizeHann(N);
        for(int i=0;i<N;++i) data[i] *= hann.at(i)*HANN_NORM;
        break;
    }
    case FFTWindow::blackman: {
        resizeBlackman(N);
        for(int i=0;i<N;++i) data[i] *= blackman.at(i)*BLACKMAN_NORM;
        break;
    }
    case FFTWindow::flatTop: {
        resizeFlatTop(N);
        for(int i=0;i<N;++i) data[i] *= flatTop.at(i)*FLATTOP_NORM;
        break;
    }
    case FFTWindow::rectangular:
    default:
        // Rectangular window = pass-through with unity normalization
        break;
    }

    int nfft = nextPow2(data.size());
    if (nfft < minNFFT)
        nfft = minNFFT;

    lengthRatio = (qreal)nfft/data.length();
    data.resize(nfft);

    return fft(data);
}

// Contract: Takes complex FFT bins (size power-of-two). Returns half-spectrum (Nyquist inclusive).
// Scaling logic matches historical behavior; TODO normalize via explicit window energy.
QVector<QPointF> FFTengine::processOutput(QVector<std::complex<float>> data, FFTType type) {
    QVector<QPointF> result;
    int nfft = data.length();
    result.reserve(nfft/2 + 1);
    float lastFreq = 0.f;
    const double freqStep = samplingFreq / nfft;
    for (int i = 0; i <= nfft / 2; i++) {
        float freq = float(i * freqStep);
        lastFreq = freq;
        if (type == FFTType::periodogram) {
            const std::complex<float> &c = data.at(i);
            const float absSquared = (c * std::complex<float>(c.real(), -c.imag())).real()/1000.0f; // |c|^2 / 1000 scaling (TODO: explain divisor)
            result.append(QPointF(freq, 10.f * log10f(absSquared/ nfft*lengthRatio)));
        } else {
            result.append(QPointF(freq, std::abs(data.at(i))/nfft*lengthRatio));
        }
    }
    maxFrequency = lastFreq;
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

void FFTengine::resizeFlatTop(int length) {
    if (flatTop.size() != length) {
        flatTop.resize(length);
        for (int n = 0; n < length; n++)
            flatTop[n] = 1 - 1.93 *cos(2* M_PI *n/(length - 1)) + 1.29 *cos(4 *M_PI *n/(length - 1)) - 0.388 *cos(6* M_PI *n/(length - 1)) + 0.0322*cos(8* M_PI *n/(length - 1));
    }

    /*

*/
}

void FFTengine::resizeBlackman(int length) {
    if (blackman.size() != length) {
        blackman.resize(length);
        for (int n = 0; n < length; n++)
            blackman[n] = 0.42 - 0.5 * cos(2 * M_PI * n / (length - 1)) + 0.08 * cos(4 * M_PI * n / (length - 1));
    }
}

int FFTengine::nextPow2(int number) {
    if (number <= 1) return 1;
    // Use bit shifting for speed; handle up to 2^30 safely for typical FFT sizes.
    int p = 1;
    while (p < number && p > 0) p <<= 1;
    return p > 0 ? p : number; // fallback in pathological overflow case
}


