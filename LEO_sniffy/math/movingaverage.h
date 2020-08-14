#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QObject>

class MovingAverage : public QObject
{
    Q_OBJECT
public:
    explicit MovingAverage(uint defaultBuffSize, QObject *parent = nullptr);
    ~MovingAverage();

    uint prepend(double newValue);
    double getAverage();
    uint setBufferSize(int buffSize);
    uint getBufferSize();
    bool isBufferFull();
    uint getSampleCountToFillBuff();
    void clearBuffer();

private:
    QList<double> buffer;
    uint buffSize;

    void removeItems(uint sampleCount);

signals:

};

#endif // MOVINGAVERAGE_H
