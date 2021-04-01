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

    SyncPwmConfig *config;
    widgetChart *chart;

    QVector<QVector<QPointF>> *points;   

    void repaint();
    void repaint(int chanIndex); 

    void enableChannel(bool enable, int chanIndex);

private:    
    qreal freq;

    qreal setTimeScale();
    void configDefaultChart();

    void recalculate();
    void recalculate(int chanIndex);

signals:

};

#endif // SYNCPWMPAINTER_H
