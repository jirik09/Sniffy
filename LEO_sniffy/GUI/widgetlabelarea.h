#ifndef WIDGETLABELAREA_H
#define WIDGETLABELAREA_H

#include <QWidget>
#include <QLabel>

#include "../modules/scope/measurement.h"

namespace Ui {
class widgetLabelArea;
}

class widgetLabelArea : public QWidget
{
    Q_OBJECT

public:
    explicit widgetLabelArea(QWidget *parent = nullptr);
    ~widgetLabelArea();

    void setTriggerLabelText(QString text);
    void setSamplingRateLabelText(QString text);
    void setScaleLabelText(QString text);
    void setMeasurements(QList<Measurement *> meas);


private:
    Ui::widgetLabelArea *ui;
    QList<QLabel*> labelList;
};

#endif // WIDGETLABELAREA_H
