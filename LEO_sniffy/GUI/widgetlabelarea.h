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

private:
    Ui::widgetLabelArea *ui;
};

#endif // WIDGETLABELAREA_H
