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

    /* Unit styles */
    void setUnitsStyle(QString &unitsStyleSheet);
    void setAvgStyle(QString &avgStyleSheet);
    void setErrStyle(QString &errStyleSheet);
    void setQerrStyle(QString &qerrStyleSheet);
    void setTerrStyle(QString &terrStyleSheet);

    /* Displays */
    void displayNumber(double number);
    void displayString(const QString &string);
    void displayAvgNumber(double number);
    void displayAvgString(const QString &string);
    void displayQerrNumber(double number);
    void displayQerrString(const QString &string);
    void displayTerrNumber(double number);
    void displayTerrString(const QString &string);
    void showQerrStyle(bool visible);
    void showTerrStyle(bool visible);
    void showQerrTerrStyle(bool visible);    
    void showAvgDisplay();
    void showErrDisplay();
    void showBarDisplay();
    void hideAvgDisplay();    
    void hideErrDisplay();
    void hideBarDisplay();
    void changeAvgColor(QColor color);

    /* Label area */
    void configLabel(int labelNumber, const QString text, QString colorStyle, bool visible);    
    void setLabelText(int labelNumber, const QString text);
    void setLabelColor(int labelNumber, const QString &textStyleSheet);
    QString getLabelText(int labelNumber);
    void hideLabel(int labelNumber);
    void showLabel(int labelNumber);
    void drawIndicationFlag(int labelNumber, QString color);

    /* Progress bar */
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
