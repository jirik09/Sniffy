#ifndef WIDGETBUTTONS_H
#define WIDGETBUTTONS_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QtMath>

namespace Ui {
class WidgetButtons;
}

enum class ButtonTypes
  {
    NORMAL,
    CHECKABLE,
    RADIO
  };

#define BACKGROUND_COLOR_BLUE   "background-color: rgb(24,154,224);"
#define BACKGROUND_COLOR_ORANGE "background-color: rgb(255,170,0);"
#define BACKGROUND_COLOR_GREEN  "background-color: rgb(54,154,24);"
#define BACKGROUND_COLOR_PURPLE "background-color: rgb(204,24,224);"

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
    int getStatus();
    void setDisabledButton(bool state, int index);
    void disableAll();
    void setChecked (bool checked, int index);

private:
    Ui::WidgetButtons *ui;
    QString buttonColors[8] = {BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE,BACKGROUND_COLOR_BLUE};
    ButtonTypes type = ButtonTypes::NORMAL;

    QList<QPushButton*> *pushButtonsList;

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
