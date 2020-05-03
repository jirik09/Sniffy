#ifndef WIDGETFOOTER_H
#define WIDGETFOOTER_H

#include <QWidget>
#include <QtWidgets/QPushButton>

namespace Ui {
class WidgetFooter;
}

class WidgetFooter : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFooter(QWidget *parent = nullptr);
    ~WidgetFooter();
    QPushButton * getPushButtonSize();


private:
    Ui::WidgetFooter *ui;

};

#endif // WIDGETFOOTER_H
