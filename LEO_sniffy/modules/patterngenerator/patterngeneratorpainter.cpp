#include "patterngeneratorpainter.h"

PatternGeneratorPainter::PatternGeneratorPainter(PatternGeneratorConfig *config, QWidget *parent) :
    QObject(parent), config(config)
{
    if(PATTERNS_USE_QCUSTOM_PLOT){
        plot = new WidgetCustomPlot(parent, PATT_MAX_CHANNELS_NUM);
    }else {
        chart = new PatternChart(parent, PATT_MAX_CHANNELS_NUM);
        chart->enableLocalMouseEvents(EventSelection::CLICKS_ONLY);
    }

    points = new QVector<QVector<QPointF>>;
    configDefaultChart();
}

void PatternGeneratorPainter::setSpecification(PatternGeneratorSpec *spec)
{
    this->spec = spec;
    if (!PATTERNS_USE_QCUSTOM_PLOT && chart && spec) {
        QStringList names;
        for (int i = 0; i < PATT_MAX_CHANNELS_NUM; ++i) names << spec->chanPins[i];
        chart->setChannelNames(names);
    }
}

void PatternGeneratorPainter::repaint(QList<quint8> *data)
{
    recalculate(data);

    if(PATTERNS_USE_QCUSTOM_PLOT){
        plot->clearAll();
        for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++){
            QVector<QPointF> vector = points->at(i);
            plot->updateTrace(&vector, i);
        }
    }else {
        chart->clearAll();
        for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++){
            QVector<QPointF> vector = points->at(i);
            chart->updateTrace(&vector, i);
        }
        // Ensure channel names are shown (spec may be set later)
        if (spec) {
            QStringList names;
            for (int i = 0; i < PATT_MAX_CHANNELS_NUM; ++i) names << spec->chanPins[i];
            chart->setChannelNames(names);
        }
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

    if(PATTERNS_USE_QCUSTOM_PLOT){
        plot->setRangeX(0, timScale);

        if(dataLen >= 32)
            plot->setGridHorizontalDensity(33);
        else
            plot->setGridHorizontalDensity(dataLen+1);
    }else {
        chart->setRangeX(0, timScale);

        if(dataLen >= 32)
            chart->setGridHorizontalDensity(33);
        else
            chart->setGridHorizontalDensity(dataLen+1);
    }        

    for(int ch = 0; ch < PATT_MAX_CHANNELS_NUM; ch++){

        log1 = PATT_RANGE_CHAN1_LOG1 - ch * 3;
        log0 = PATT_RANGE_CHAN1_LOG0 - ch * 3;
        stepSum = 0;

        QVector<QPointF> tmpVector;

        for(int d = 0; d < dataLen; d++){

            if(data->at(d) & (1 << ch)){
                tmpVector.append(QPointF(stepSum+step/10000, log1));
                tmpVector.append(QPointF(stepSum+step, log1));
            }else {
                tmpVector.append(QPointF(stepSum+step/10000, log0));
                tmpVector.append(QPointF(stepSum+step, log0));
            }
            stepSum += step;
        }

        points->append(tmpVector);
    }
}

void PatternGeneratorPainter::configDefaultChart(){    
    if(PATTERNS_USE_QCUSTOM_PLOT){
    plot->setGraphColor(QColor(Graphics::palette().chartGridlegLowContrast));
        plot->setGridHorizontalDensity(PATT_DEFAULT_DATA_LENGTH+1);
        plot->setLabelsVisible(true, false);
        plot->setRange(0, 1 , 0.97, PATT_RANGE_CHAN1_LOG1 + 0.98);
        for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++)
            plot->setTraceColor(i, Graphics::getChannelColor(0));
    }else {
        chart->setGridHorizontalDensity(PATT_DEFAULT_DATA_LENGTH+1);
        chart->setLabelsVisible(true, false);
        chart->setRange(0, 1, 0.97, PATT_RANGE_CHAN1_LOG1 + 0.98);
        for(int i = 0; i < PATT_MAX_CHANNELS_NUM; i++)
            chart->setTraceColor(i, Graphics::getChannelColor(0));
    }
}
