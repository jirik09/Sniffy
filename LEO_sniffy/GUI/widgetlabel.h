#ifndef WIDGETLABEL_H
#define WIDGETLABEL_H

#include <QWidget>

namespace Ui {
class WidgetLabel;
}

class WidgetLabel : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetLabel(QWidget *parent = nullptr, QString name = "");
    ~WidgetLabel();

private:
    Ui::WidgetLabel *ui;
};

#endif // WIDGETLABEL_H
