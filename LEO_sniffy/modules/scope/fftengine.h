#ifndef FFTENGINE_H
#define FFTENGINE_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <complex>
#include <QPointF>
#include <QDebug>

#include "fftdefs.h" // central enum definitions

class FFTengine : public QThread
{
    Q_OBJECT
public:
    explicit FFTengine(QObject *parent = nullptr);
    ~FFTengine();

    Q_DISABLE_COPY(FFTengine)
    // Thread objects should not be moved either once started; explicitly delete moves.
    FFTengine(FFTengine&&) = delete;
    FFTengine& operator=(FFTengine&&) = delete;

    void calculate(QVector<QPointF> data,FFTWindow window,FFTType type, int minNFFT,bool removeDC, qreal samplingFreq);
    QVector<QPointF> getProcessedData();
    qreal getMaxFrequency();

protected:
    void run() override;

private:
    //thread stuff
    QMutex mutex;
    QWaitCondition condition;
    bool restart;
    bool abort;

    // Pre-sized reusable window buffers (grown on demand). Avoids reallocations inside loop.


    QVector<float> hamming, hann, blackman,flatTop;

    void resizeHamming(int length);
    void resizeHann(int length);
    void resizeFlatTop(int length);
    void resizeBlackman(int length);
    int nextPow2(int number);
    QVector<std::complex<float>> calculateSpectrum(QVector<std::complex<float>> data, FFTWindow window, int minNFFT);
    QVector<QPointF> processOutput(QVector<std::complex<float>> data, FFTType type);
    QVector<std::complex<float>> fft(QVector<std::complex<float>> x);

    QVector<QPointF> dataIn;
    QVector<std::complex<float>> outputSpectrum;
    QVector<QPointF> outputData;
    FFTWindow window;
    qreal samplingFreq;
    qreal maxFrequency;
    qreal lengthRatio;
    FFTType type;
    int lengthNFFT;
    bool removeDC;




signals:
    void fftCalculated();


};

#endif // FFTENGINE_H
