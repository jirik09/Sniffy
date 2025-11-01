#ifndef PATTERNGENERATORPAINTER_H
#define PATTERNGENERATORPAINTER_H

#include <QObject>

#include "patternchart.h"
#include "../../GUI/widgetcustomplot.h"

#include "patterngeneratorconfig.h"
#include "patterngeneratorpatterns.h"
#include "patterngeneratorspec.h"

class PatternGeneratorPainter : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorPainter(PatternGeneratorConfig *config, QWidget *parent = nullptr);
    void setSpecification(PatternGeneratorSpec *spec);

    void repaint(QList<quint8> *data);

    WidgetCustomPlot *plot;
    PatternChart *chart;

private:
    PatternGeneratorConfig *config;
    PatternGeneratorSpec *spec = nullptr;

    QVector<QVector<QPointF>> *points;

    void recalculate(QList<quint8> *data);
    void configDefaultChart();

signals:

};

#endif // PATTERNGENERATORPAINTER_H
