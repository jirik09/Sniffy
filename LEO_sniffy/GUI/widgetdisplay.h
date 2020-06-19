#ifndef WIDGETDISPLAY_H
#define WIDGETDISPLAY_H

#include <QWidget>

namespace Ui {
class WidgetDisplay;
}

class WidgetDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDisplay(QWidget *parent = nullptr);
    ~WidgetDisplay();

private:
    Ui::WidgetDisplay *ui;
};

#endif // WIDGETDISPLAY_H
