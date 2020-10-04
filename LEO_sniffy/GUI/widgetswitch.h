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
    explicit WidgetSwitch(QWidget *parent = nullptr, QString leftName = "", QString rightName = "", QString name = "");
    ~WidgetSwitch();
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);

    bool isCheckedRight();
    bool isCheckedLeft();

private:
    Ui::WidgetSwitch *ui;

    const QString SELECTED_STYLE = "background-color: rgb(24,154,224);border: none;";
    const QString NOT_SELECTED_STYLE = "background-color: rgb(38,38,38);border: none;";    

signals:
    void clicked(int index);

private slots:
    void setRight();
    void setLeft();
};

#endif // WIDGETSWITCH_H
