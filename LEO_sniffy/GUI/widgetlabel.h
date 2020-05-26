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
    explicit WidgetLabel(QWidget *parent = nullptr, QString name = "", QString value = "");
    ~WidgetLabel();

    void setName(QString name);
    void setValue(QString value);

private:
    Ui::WidgetLabel *ui;
};

#endif // WIDGETLABEL_H
