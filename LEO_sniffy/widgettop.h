#ifndef WIDGETTOP_H
#define WIDGETTOP_H

#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui {
class WidgetTop;
}

#define SELECTED_STYLE "background-color: rgb(58,58,58);border: none;border-bottom: 3px solid rgb(255, 170, 0);"
#define NOT_SELECTED_STYLE "background-color: rgb(38,38,38);border: none;border-bottom: 3px solid rgb(124, 124, 124);"

class WidgetTop : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTop(QWidget *parent = nullptr, int num = 1);
    ~WidgetTop();
    void setText(QString text, int index);
    void setImage(QString uri, int index);
    void setSelected (int index);
    void clearSelected (void);
    QPushButton * getPushButton(int index);

private:
    Ui::WidgetTop *ui;
};

#endif // WIDGETTOP_H
