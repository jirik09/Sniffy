#ifndef WIDGETSWITCH_H
#define WIDGETSWITCH_H

#include <QWidget>

namespace Ui {
class WidgetSwitch;
}




class WidgetSwitch : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSwitch(QWidget *parent = nullptr, QString name = "");
    ~WidgetSwitch();

private:
    Ui::WidgetSwitch *ui;

    const QString SELECTED_STYLE = "background-color: rgb(24,154,224);border: none;";
    const QString NOT_SELECTED_STYLE = "background-color: rgb(38,38,38);border: none;";

private slots:
    void setOn();
    void setOff();
};

#endif // WIDGETSWITCH_H
