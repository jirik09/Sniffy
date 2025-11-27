#ifndef MEASCALCULATIONS_H
#define MEASCALCULATIONS_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QPointF>
#include <QtMath>
#include <QDebug>

#include "measurement.h"
#include "../labelformator.h"

class MeasCalculations: public QThread
{
    Q_OBJECT
public:
    MeasCalculations(QObject *parent = nullptr);
    ~MeasCalculations();

    void calculate(QVector<QVector<QPointF>> scopeData, QList<Measurement*> scopeMeas, qint32 samplingFreq);

signals:
    void measCalculated(QList<Measurement *> list);

protected:
    void run() override;

private:

    QVector<QVector<QPointF>> data;
    QList<Measurement*> meas;
    qint32 samplingFreq;

    bool isVoltCalculated[8] = {0};
    qreal RMS[8] = {0};
    qreal Mean[8] = {0};
    qreal Max[8] = {0};
    qreal Min[8] = {0};

    bool isTimeCalculated[8] = {0};
    qreal Period[8] = {0};
    qreal Freq[8] = {0};
    qreal High[8] = {0};

    #define MAX_ZERO_CROSS 128
    int ZeroCrossingTimes[8][MAX_ZERO_CROSS] = {{0,0}};
    int ZeroCrossingCount[8] = {0};

    QMutex mutex;
    QWaitCondition condition;
    bool restart;
    bool abort;

    void calculateTime(QVector<QPointF> data, qint32 channel, qint32 sampling);
    void calculateVolt(QVector<QPointF> data, qint32 channel);

};

#endif // MEASCALCULATIONS_H
