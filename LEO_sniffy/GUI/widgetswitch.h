#ifndef WIDGETSWITCH_H
#define WIDGETSWITCH_H

#include <QWidget>

namespace Ui {
class WidgetSwitch;
}

#define SELECTED_STYLE "background-color: rgb(24,154,224);border: none;"
#define NOT_SELECTED_STYLE "background-color: rgb(38,38,38);border: none;"

class WidgetSwitch : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSwitch(QWidget *parent = nullptr, QString name = "");
    ~WidgetSwitch();

private:
    Ui::WidgetSwitch *ui;

private slots:
    void setOn();
    void setOff();
};

#endif // WIDGETSWITCH_H
