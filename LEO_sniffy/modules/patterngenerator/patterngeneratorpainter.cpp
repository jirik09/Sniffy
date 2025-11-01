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
    // Normalize X to 0..1 so visual grid width is identical across all patterns
    // regardless of the generator's frequency value.
    qreal step = 1.0 / dataLen;
    qreal stepSum;
    int log0, log1;

    if(PATTERNS_USE_QCUSTOM_PLOT){
        plot->setRangeX(0, 1);
        plot->setGridHorizontalDensity(PATT_DEFAULT_DATA_LENGTH + 1);
    }else {
        chart->setRangeX(0, 1);
        chart->setGridHorizontalDensity(PATT_DEFAULT_DATA_LENGTH + 1);
        // Use a consistent label format to avoid variable label widths affecting plot area
        chart->formatLabels("%g", "%g");
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
    // Common configuration values
    const int gridH = PATT_DEFAULT_DATA_LENGTH + 1;
    const int gridV = PATT_MAX_CHANNELS_NUM + 1;
    const bool labelsX = true;
    const bool labelsY = false;
    const int leftMargin = 0, topMargin = 0, rightMargin = 0, bottomMargin = 0;
    const qreal xMin = 0, xMax = 1;
    const qreal yMin = 0.97, yMax = PATT_RANGE_CHAN1_LOG1 + 0.98;

    // Backend-specific setup
    if (PATTERNS_USE_QCUSTOM_PLOT) {
        plot->setGraphColor(QColor(Graphics::palette().chartGridlegLowContrast));
        plot->setGridDensity(gridH, gridV);
        plot->setLabelsVisible(labelsX, labelsY);
        plot->setContentsMargins(leftMargin, topMargin, rightMargin, bottomMargin);
        plot->setRange(xMin, xMax, yMin, yMax);
    } else {
        chart->setGridDensity(gridH, gridV);
        chart->setLabelsVisible(labelsX, labelsY);
        chart->setMargins(leftMargin, topMargin, rightMargin, bottomMargin);
        chart->setRange(xMin, xMax, yMin, yMax);
    }

    // Set trace colors for all traces (single loop, backend-agnostic via lambda)
    auto setTraceColor = [&](int idx){
        if (PATTERNS_USE_QCUSTOM_PLOT)
            plot->setTraceColor(idx, Graphics::getChannelColor(0));
        else
            chart->setTraceColor(idx, Graphics::getChannelColor(0));
    };

    for (int i = 0; i < PATT_MAX_CHANNELS_NUM; ++i)
        setTraceColor(i);
}
