#ifndef WIDGETDISPLAY_H
#define WIDGETDISPLAY_H

#include <QWidget>
#include <QLabel>

#include "widgetlabel.h"

namespace Ui {
class WidgetDisplay;
}

class WidgetDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDisplay(QString firstLabelText, QString units, bool showPrgrssBar, QWidget *parent = nullptr);
    ~WidgetDisplay();

private:
    QList<QLabel*> labelList;

protected:           
    void setUnits(QString units);
    void displayNumber(double number);
    QString getLabelText(int labelNumber);
    void setLabelText(int labelNumber, const QString text);

    void hideProgressBar();
    void showProgressBar();
    void hideLabel(int labelNumber);
    void showLabel(int labelNumber);

private:
    Ui::WidgetDisplay *ui;
};

#endif // WIDGETDISPLAY_H
