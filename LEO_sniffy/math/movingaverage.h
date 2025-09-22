#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <QObject>
#include <QVector>

/*
 * MovingAverage
 * Simple fixed-size moving average over recent numeric samples.
 * Uses QList prepend/removeLast (O(n)) which is fine for small buffers; could be
 * optimized to a circular buffer if higher throughput needed.
 * Invariants:
 *  - buffSize >= 1
 *  - buffer.count() <= buffSize
 * getAverage() now returns NaN when buffer not full (previously magic -100).
 */
class MovingAverage : public QObject
{
    Q_OBJECT
public:
    explicit MovingAverage(uint defaultBuffSize = 2, QObject *parent = nullptr);
    ~MovingAverage();

    // Add a new sample (most recent). Returns remaining samples to fill the buffer.
    uint prepend(double newValue);
    // Average over current window (partial if not yet full). Returns NaN if empty.
    double getAverage();
    // Explicit full-window average; returns NaN until buffer full.
    double getAverageFull() const;
    // Average over current (possibly partial) window; identical to getAverage() but const.
    double getAveragePartial() const; // kept for clarity/backward extension
    uint setBufferSize(int buffSize);
    uint getBufferSize() const;
    bool isBufferFull() const;
    bool hasFullWindow() const { return isBufferFull(); }
    uint getSampleCountToFillBuff() const;
    void clear();

private:
    QVector<double> buffer;        // circular storage
    uint buffSize {0};             // target window size
    uint count {0};                // number of valid samples currently stored
    uint head {0};                 // index of most recently inserted element
    double runningSum {0.0};       // sum of current valid samples

    void allocate(uint size);
    inline uint physicalIndex(uint logicalIndex) const { // 0 = newest, logicalIndex < count
        return (head + buffer.size() - logicalIndex) % buffer.size();
    }

signals:

};

#endif // MOVINGAVERAGE_H
