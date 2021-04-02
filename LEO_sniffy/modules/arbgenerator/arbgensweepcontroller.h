#ifndef ARBGENSWEEPCONTROLLER_H
#define ARBGENSWEEPCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

#include "arbgeneratordefs.h"

class ArbGenSweepController : public QObject
{
    Q_OBJECT
public:
    explicit ArbGenSweepController(QObject *parent = nullptr);

    void setParameters(qreal minFreq,qreal maxFreq,qreal totalTime);
    void startTimer();
    void stopTimer();

private:
    qreal minFreq;
    qreal maxFreq;
    qreal totalTime;

    qreal sampleTime;
    int numSamples;
    QVector<qreal> freqSamples;
    int actualIndex = 0;
    bool isRunning = false;

    QTimer *timer;

signals:
    void updateSweepFrequency(qreal frequency);

private slots:
    void timerCallback();

};

#endif // ARBGENSWEEPCONTROLLER_H
