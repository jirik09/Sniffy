#ifndef MATHCALCULATIONS_H
#define MATHCALCULATIONS_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QPointF>
#include <QtMath>
#include <QDebug>

#include "../../math/tinyexpr.h"

class MathCalculations : public QThread
{
    Q_OBJECT
public:
    explicit MathCalculations(QObject *parent = nullptr);
    ~MathCalculations();

    void calculate(QVector<QVector<QPointF>> scopeData, qint32 samplingFreq, QString expression);
    QVector<QPointF> getCalculatedMathTrace();
signals:
    void mathCalculated(int errorPosition);

protected:
    void run() override;

    int calcSymbolic(QVector<QVector<QPointF>> inData, QVector<QPointF>  &outData,QString expression);

private:
    QVector<QVector<QPointF>> data;
    QVector<QPointF> outputData;
    qint32 samplingFreq;
    QString expression;
    int errorPosition;

    QMutex mutex;
    QWaitCondition condition;
    bool restart;
    bool abort;

};

#endif // MATHCALCULATIONS_H
