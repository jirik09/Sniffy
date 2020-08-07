#ifndef WIDGETDISPLAY_H
#define WIDGETDISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QString>

#include "widgetlabel.h"
#include "../graphics/colors.h"

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
    void setQerrStyle(QString &qerrStyleSheet);
    void setTerrStyle(QString &terrStyleSheet);

    void displayNumber(double number);
    void displayString(const QString &string);
    void displayAvgNumber(double number);
    void displayAvgString(const QString &string);
    void displayQerrNumber(double number);
    void displayQerrString(const QString &string);
    void displayTerrNumber(double number);
    void displayTerrString(const QString &string);

    void changeAvgColor(QColor color);

    void configLabel(int labelNumber, const QString text, QString colorStyle, bool visible);
    QString getLabelText(int labelNumber);
    void setLabelText(int labelNumber, const QString text);
    void hideLabel(int labelNumber);
    void showLabel(int labelNumber);
    void drawFlagLabel(int labelNumber);

    void hideProgressBar();
    void showProgressBar();
    void setRangeProgressBar(int min, int max);
    void updateProgressBar(int value);

private:
    Ui::WidgetDisplay *ui;
    QList<QLabel*> labelList;
    QPalette palette;
    int drawFlag = 0;
};

#endif // WIDGETDISPLAY_H
