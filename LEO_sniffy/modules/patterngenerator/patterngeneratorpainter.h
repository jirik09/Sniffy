#ifndef PATTERNGENERATORPAINTER_H
#define PATTERNGENERATORPAINTER_H

#include <QObject>

#include "../../GUI/widgetchart.h"
#include "../../GUI/widgetcustomplot.h"

#include "patterngeneratorconfig.h"
#include "patterngeneratorpatterns.h"

class PatternGeneratorPainter : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorPainter(PatternGeneratorConfig *config, QWidget *parent = nullptr);

    void repaint(QList<quint8> *data);

    WidgetCustomPlot *plot;
    widgetChart *chart;

private:
    PatternGeneratorConfig *config;

    QVector<QVector<QPointF>> *points;

    void recalculate(QList<quint8> *data);
    void configDefaultChart();

signals:

};

#endif // PATTERNGENERATORPAINTER_H
