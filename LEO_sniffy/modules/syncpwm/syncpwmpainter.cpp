#include "syncpwmpainter.h"

SyncPwmPainter::SyncPwmPainter(widgetChart *chart, SyncPwmConfig *config, QObject *parent) :
    QObject(parent), config(config), chart(chart)
{
    points = new QVector<QVector<QPointF>>;
    configDefaultChart();
}

void SyncPwmPainter::repaint(){
    recalculate();
    chart->clearAll();
    for(int i = 0; i < CHANNELS_NUM; i++){
        QVector<QPointF> vector = points->at(i);
        chart->updateTrace(&vector, i);
    }
}

void SyncPwmPainter::recalculate(){
    points->clear();

    qreal timScale = setTimeScale();
    qreal yLog0, yLog1, sumPer, chPer;

    for(int i = 0; i < CHANNELS_NUM; i++){

        if(config->chan[i].inverted){
            yLog1 = RANGE_CHAN1_LOG0 - i * 3;
            yLog0 = RANGE_CHAN1_LOG1 - i * 3;
        }else {
            yLog0 = RANGE_CHAN1_LOG0 - i * 3;
            yLog1 = RANGE_CHAN1_LOG1 - i * 3;
        }

        sumPer = 0;
        chPer = 1 / config->chan[i].freq;
        qreal stepNum = timScale/chPer;
        if(stepNum > MAX_PAINT_PERIODS){
            stepNum = MAX_PAINT_PERIODS;
            chPer = timScale / stepNum;
        }

        QVector<QPointF> tmpVector;

        bool allLog0 = (config->chan[i].duty==0 || config->chan[i].phase==360) ? true : false;
        bool allLog1 = (config->chan[i].duty==100 && config->chan[i].phase==0) ? true : false;

        if(allLog0){
            yLog1 = yLog0;
        }else if(allLog1) {
            yLog0 = yLog1;
        }

        qreal edge1 = config->chan[i].phase / (360 * config->chan[i].freq);
        qreal edge2 = edge1 + config->chan[i].duty / (100 * config->chan[i].freq);

        if(edge2 > chPer)
            edge2 = chPer;

        tmpVector.append(QPointF(sumPer, yLog0));

        int repShift = 0;
        if(config->common.stepMode){
            tmpVector.append(QPointF(edge1+sumPer, yLog0));
            tmpVector.append(QPointF(edge1+sumPer, yLog1));
            tmpVector.append(QPointF(edge2+sumPer, yLog1));
            tmpVector.append(QPointF(edge2+sumPer, yLog0));
            sumPer += chPer;
            repShift = 1;
            yLog1 = yLog0;
        }

        for(int rep = repShift; rep < stepNum; rep++){
            tmpVector.append(QPointF(edge1+sumPer, yLog0));
            tmpVector.append(QPointF(edge1+sumPer, yLog1));
            tmpVector.append(QPointF(edge2+sumPer, yLog1));
            tmpVector.append(QPointF(edge2+sumPer, yLog0));
            sumPer += chPer;
        }

        tmpVector.append(QPointF(chPer+sumPer, yLog0));
        points->append(tmpVector);
    }
}

void SyncPwmPainter::enableChannel(bool enable, int chanIndex){
    if(enable){
        switch(chanIndex){
        case 0:
            chart->setTraceColor(0, QCOLOR_CHAN1);
            break;
        case 1:
            chart->setTraceColor(1, QCOLOR_CHAN2);
            break;
        case 2:
            chart->setTraceColor(2, QCOLOR_CHAN3);
            break;
        case 3:
            chart->setTraceColor(3, QCOLOR_CHAN4);
            break;
        default:
            break;
        }
    }else {
        chart->setTraceColor(chanIndex, QCOLOR_GREY);
    }

    repaint();
}

qreal SyncPwmPainter::setTimeScale(){
    freq = config->chan[0].freq;
    for(int i = 1; i < CHANNELS_NUM; i++)
        if(config->chan[i].freq < freq)
            freq = config->chan[i].freq;

    qreal period = 1/freq;
    chart->setRangeX(0, period);

    return period;
}

void SyncPwmPainter::configDefaultChart(){
    chart->setGraphColor(QCOLOR_GREY);
    chart->setMargins(-12, -5, -6, -10);
    chart->setLabelsVisible(true, false);
    chart->setRange(0, 1, 0, RANGE_CHAN1_LOG1 + 1);
    chart->setTraceColor(0, QCOLOR_CHAN1);
    chart->setTraceColor(1, QCOLOR_CHAN2);
    chart->setTraceColor(2, QCOLOR_CHAN3);
    chart->setTraceColor(3, QCOLOR_CHAN4);
}

