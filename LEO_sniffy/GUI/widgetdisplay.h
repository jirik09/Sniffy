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
    explicit WidgetDisplay(QString firstLabelText, QString &unitsStyleSheet, bool showPrgrssBar, QWidget *parent = nullptr);
    ~WidgetDisplay();

    void setUnitsStyle(QString &unitsStyleSheet);
    void setAvgStyle(QString &avgStyleSheet);
    void setErrStyle(QString &errStyleSheet);
    void displayNumber(double number);
    void displayNumber(const QString &string);
    QString getLabelText(int labelNumber);
    void setLabelText(int labelNumber, const QString text);

    void hideProgressBar();
    void showProgressBar();
    void hideLabel(int labelNumber);
    void showLabel(int labelNumber);

private:
    Ui::WidgetDisplay *ui;
    QList<QLabel*> labelList;
};

#endif // WIDGETDISPLAY_H
