#include "mathcalculations.h"

MathCalculations::MathCalculations(QObject *parent) : QThread(parent)
{
    restart = false;
    abort = false;
}

MathCalculations::~MathCalculations()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void MathCalculations::calculate(QVector<QVector<QPointF> > scopeData, qint32 samplingFreq, QString expression)
{
    QMutexLocker locker(&mutex);

    data = scopeData;
    this->expression = expression;
    this->samplingFreq = samplingFreq;
    if (!isRunning()) {
        start(NormalPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

QVector<QPointF> MathCalculations::getCalculatedMathTrace()
{
    return  outputData;
}

void MathCalculations::run()
{
    QVector<QPointF> tmpTrace;
    forever {
        mutex.lock();
        QVector<QVector<QPointF>> data = this->data;
        tmpTrace.clear();
        QString expression = this->expression;
        qint32 samplingFreq = this->samplingFreq;
        mutex.unlock();

        if(expression == "")return;
       // qDebug () << expression;

        this->errorPosition=calcSymbolic(data,tmpTrace,expression);

        if (restart)
            break;
        if (abort)
            return;

        if (!restart){
            outputData = tmpTrace;
            emit mathCalculated(errorPosition);
        }
        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
    }
}


int MathCalculations::calcSymbolic(QVector<QVector<QPointF> > inData, QVector<QPointF>  &outData,QString expression){
    QVector<QPointF>  tmpData;
    int variableCount = inData.length()+1;
    double t,a,b,c,d;

    if(variableCount<2){
        return -1;
    }

    te_variable vars[variableCount];
    if(variableCount>=2){
        vars[0]= {"t", &t};
        vars[1]= {"a", &a};
    }
    if(variableCount>=3)vars[2]= {"b", &b};
    if(variableCount>=4)vars[3]= {"c", &c};
    if(variableCount>=5)vars[4]= {"d", &d};

    std::string ex = expression.toStdString();
    const char *expChar = ex.data();
    /* This will compile the expression and check for errors. */
    int err;
    te_expr *n = te_compile(expChar, vars, variableCount, &err);

    for (int i = 0;i<inData[0].length();i++) {
        if(variableCount>=2){
            t = inData[0][i].x();
            a = inData[0][i].y();
        }
        if(variableCount>=3)b = inData[1][i].y();
        if(variableCount>=4)c = inData[2][i].y();
        if(variableCount>=5)d = inData[3][i].y();

        tmpData.append(QPointF(inData[0][i].x(),te_eval(n)));
    }
    te_free(n);
    outData = tmpData;
    return  err;
}
