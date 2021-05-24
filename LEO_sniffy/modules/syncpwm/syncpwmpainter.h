#ifndef SYNCPWMPAINTER_H
#define SYNCPWMPAINTER_H

#include <QObject>

#include "../../GUI/widgetchart.h"

#include "syncpwmconfig.h"
#include "syncpwmdefs.h"

class SyncPwmPainter : public QObject
{
    Q_OBJECT
public:
    explicit SyncPwmPainter(widgetChart *chart, SyncPwmConfig *config, QObject *parent = nullptr);       

    void repaint();
    void enableChannel(bool enable, int chanIndex);

private:
    SyncPwmConfig *config;
    widgetChart *chart;
    QVector<QVector<QPointF>> *points;
    qreal freq;

    qreal setTimeScale();
    void configDefaultChart();

    void recalculate();    

signals:

};

#endif // SYNCPWMPAINTER_H
