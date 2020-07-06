#ifndef WIDGETDISPLAY_H
#define WIDGETDISPLAY_H

#include <QWidget>

#include "widgetlabel.h"

namespace Ui {
class WidgetDisplay;
}

class WidgetDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDisplay(bool showPrgrssBar, QString units, QString label_1, QString label_2, QString label_3, QWidget *parent = nullptr);
    ~WidgetDisplay();

protected:
    void setUnits(QString units);
    void displayNumber(double number);
    QString getLabelText(int labelNumer);
    void setLabelText(int labelNumer, const QString text);

    void hideProgressBar();
    void showProgressBar();
    void hideLabel(int labelNumer);
    void showLabel(int labelNumer);

private:
    Ui::WidgetDisplay *ui;
};

#endif // WIDGETDISPLAY_H
