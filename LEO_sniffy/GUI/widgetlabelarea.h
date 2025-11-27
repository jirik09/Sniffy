#ifndef WIDGETLABELAREA_H
#define WIDGETLABELAREA_H

#include <QWidget>
#include <QLabel>

#include "../modules/scope/measurement.h"
#include "../graphics/graphics.h"
#include "../modules/labelformator.h"

namespace Ui {
class widgetLabelArea;
}

class WidgetLabelArea : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetLabelArea(QWidget *parent = nullptr);
    ~WidgetLabelArea();

    void setChannelLabelVisible(int index, bool isVisible);
    void setChannelScale(int index, QString Scale);
    void setTriggerLabelText(QString text);
    void setSamplingRateLabelText(QString text);
    void setScaleLabelText(QString text);
    void setMeasurements(QList<Measurement *> meas);
    void setCursorVoltageReadings(qreal curA, qreal curB);
    void setCursorVoltageReadingsX(qreal curA, qreal curB);
    void setCursorTimeReadings(qreal curA, qreal curB);
    void setCursorFFTFreqReadings(qreal curA, qreal curB);
    void setCursorFFTAmplitudeReadings(qreal curA, qreal curB, QString unit);
    void setCursorReadingsColor(int channelIndex);
    void hideCursorReadings();
    void hideCursorReadingsHor();

    void hideChannelLabels();


private:
    Ui::widgetLabelArea *ui;
    QList<QLabel*> measLabelList;
    QList<QLabel*> channLabelList;
};

#endif // WIDGETLABELAREA_H
