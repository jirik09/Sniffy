#ifndef WIDGETSEPARATOR_H
#define WIDGETSEPARATOR_H

#include <QWidget>

namespace Ui {
class WidgetSeparator;
}

class WidgetSeparator : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSeparator(QWidget *parent = nullptr, QString name = "");
    ~WidgetSeparator();

    void hideLine();
    void setText(QString text);

private:
    Ui::WidgetSeparator *ui;
};

#endif // WIDGETSEPARATOR_H
