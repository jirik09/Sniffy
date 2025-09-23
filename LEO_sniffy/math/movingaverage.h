#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QObject>
#include <QList>

class MovingAverage : public QObject
{
    Q_OBJECT
public:
    explicit MovingAverage(uint defaultBuffSize = 2, QObject *parent = nullptr);
    ~MovingAverage();

    uint prepend(double newValue);
    double getAverage();
    uint setBufferSize(int buffSize);
    uint getBufferSize();
    bool isBufferFull();
    uint getSampleCountToFillBuff();
    void clear();

private:
    QList<double> buffer;      // newest element at index 0
    uint buffSize;             // fixed target window size
    double runningSum {0.0};   // sum of current elements (valid when buffer.count() > 0)

    void removeItems(uint sampleCount);
    void clearBuffer();

signals:

};

#endif // MOVINGAVERAGE_H