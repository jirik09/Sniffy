#ifndef WIDGETLABELAREA_H
#define WIDGETLABELAREA_H

#include <QWidget>

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


private:
    Ui::widgetLabelArea *ui;
};

#endif // WIDGETLABELAREA_H
