#include "patterngeneratorpainter.h"

PatternGeneratorPainter::PatternGeneratorPainter(widgetChart *chart, PatternGeneratorConfig *config, QObject *parent) :
    QObject(parent), config(config), chart(chart)
{
    points = new QVector<QVector<QPointF>>;
    configDefaultChart();
}

void PatternGeneratorPainter::repaint(QList<quint8> *data)
{
    recalculate(data);

    chart->clearAll();
    for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++){
        QVector<QPointF> vector = points->at(i);
        chart->updateTrace(&vector, i);
    }
}

void PatternGeneratorPainter::recalculate(QList<quint8> *data)
{
    points->clear();

    qreal dataLen = data->length();    
    qreal timScale = 1 / config->freq[config->pattIndex];
    qreal step = timScale / dataLen;
    qreal stepSum;
    int log0, log1;

    chart->setRangeX(0, timScale);

    if(dataLen >= 32)
        chart->setGridHorizontalDensity(33);
    else
        chart->setGridHorizontalDensity(dataLen+1);

    for(int ch = 0; ch < PATT_MAX_CHANNELS_NUM; ch++){

        log1 = PATT_RANGE_CHAN1_LOG1 - ch * 3;
        log0 = PATT_RANGE_CHAN1_LOG0 - ch * 3;
        stepSum = 0;

        QVector<QPointF> tmpVector;

        for(int d = 0; d < dataLen; d++){

            if(data->at(d) & (1 << ch)){
                tmpVector.append(QPointF(stepSum, log1));
                tmpVector.append(QPointF(stepSum+step, log1));
            }else {
                tmpVector.append(QPointF(stepSum, log0));
                tmpVector.append(QPointF(stepSum+step, log0));
            }
            stepSum += step;
        }

        points->append(tmpVector);
    }
}

void PatternGeneratorPainter::configDefaultChart(){    
    chart->setGridHorizontalDensity(PATT_DEFAULT_DATA_LENGTH+1);
    chart->setMargins(-12, -5, -6, -4);
    chart->setLabelsVisible(true, false);
    chart->setRange(0, 1, 0.97, PATT_RANGE_CHAN1_LOG1 + 0.98);
    for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++)
        chart->setTraceColor(i, Graphics::getChannelColor(0));
}
