#ifndef PATTERNGENERATORPAINTER_H
#define PATTERNGENERATORPAINTER_H

#include <QObject>

#include "../../GUI/widgetchart.h"

#include "patterngeneratorconfig.h"
#include "patterngeneratorpatterns.h"

class PatternGeneratorPainter : public QObject
{
    Q_OBJECT
public:
    explicit PatternGeneratorPainter(widgetChart *chart, PatternGeneratorConfig *config, QObject *parent = nullptr);

    void repaint(QList<quint8> *data);

private:
    PatternGeneratorConfig *config;
    widgetChart *chart;

    QVector<QVector<QPointF>> *points;

    void recalculate(QList<quint8> *data);
    void configDefaultChart();

signals:

};

#endif // PATTERNGENERATORPAINTER_H
