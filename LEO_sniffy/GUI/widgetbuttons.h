#ifndef WIDGETBUTTONS_H
#define WIDGETBUTTONS_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QtMath>

#include "../graphics/colors.h"

namespace Ui {
class WidgetButtons;
}

enum class ButtonTypes
  {
    NORMAL,
    CHECKABLE,
    RADIO
  };



//#define SELECTED_STYLE "border: none;"

#define NOT_CHECKED_BACKGROUND "background-color: rgb(38,38,38);"

class WidgetButtons : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetButtons(QWidget *parent = nullptr, int num = 1, ButtonTypes type = ButtonTypes::NORMAL, QString name = "", int defaultSelectedIndex = 0);
    ~WidgetButtons();
    void setText(QString text, int index = 0);
    QString getText(int index);
    void setColor(QString text, int index);
    void clickedInternal (int index);
    bool isChecked (int index);
    int getSelectedIndex();
    int getStatus();
    void setDisabledButton(bool disabled, int index);
    void setEnabledButton(bool enabled, int index);
    void disableAll();
    void enableAll();
    void setChecked (bool checked, int index);

private:
    Ui::WidgetButtons *ui;
    QString buttonColors[8] = {BCKGRND_COLOR_BLUE};
    ButtonTypes type = ButtonTypes::NORMAL;

    QList<QPushButton*> pushButtonsList;

    void uncheckAll();

private slots:
    void button_1_Clicked();
    void button_2_Clicked();
    void button_3_Clicked();
    void button_4_Clicked();
    void button_5_Clicked();
    void button_6_Clicked();
    void button_7_Clicked();
    void button_8_Clicked();


signals:
    void clicked(int index);
    void statusChanged(int status);
};

#endif // WIDGETBUTTONS_H
