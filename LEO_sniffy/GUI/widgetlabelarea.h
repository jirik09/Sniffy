#ifndef WIDGETLABELAREA_H
#define WIDGETLABELAREA_H

#include <QWidget>
#include <QLabel>

#include "../modules/scope/measurement.h"
#include "../graphics/colors.h"

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
    void setTriggerLabelText(QString text);
    void setSamplingRateLabelText(QString text);
    void setScaleLabelText(QString text);
    void setMeasurements(QList<Measurement *> meas);
    void hideChannelLabels();


private:
    Ui::widgetLabelArea *ui;
    QList<QLabel*> measLabelList;
    QList<QLabel*> channLabelList;
};

#endif // WIDGETLABELAREA_H
