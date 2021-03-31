#ifndef WIDGETSWITCH_H
#define WIDGETSWITCH_H

#include <QWidget>

#include "../graphics/colors.h"

namespace Ui {
class WidgetSwitch;
}

class WidgetSwitch : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSwitch(QWidget *parent = nullptr, QString leftName = "", QString rightName = "", QString name = "", int optionalEmitParam = 0);
    ~WidgetSwitch();
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);

    bool isCheckedRight();
    bool isCheckedLeft();

    void setColor(QString color);    

private:
    Ui::WidgetSwitch *ui;
    int optionalEmitParam;

    QString styleSelected =
            QString::fromUtf8("background-color:") + COLOR_BLUE + QString::fromUtf8("border: none;");
    QString styleNotSelected =
            QString::fromUtf8("background-color:") + BACKGROUND_COLOR_BUTTON + QString::fromUtf8("border: none;");

signals:
    void clicked(int index, int optionalEmitParam = 0);

private slots:
    void setRight(bool silent = false);
    void setLeft(bool silent = false);
};

#endif // WIDGETSWITCH_H
