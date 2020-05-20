#ifndef WIDGETSELECTION_H
#define WIDGETSELECTION_H

#include <QWidget>

namespace Ui {
class WidgetSelection;
}

typedef struct
{
    QString shownValue;
    float realValue;
} params_sel;

class WidgetSelection : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSelection(QWidget *parent = nullptr, QString name = "");
    ~WidgetSelection();

private:
    Ui::WidgetSelection *ui;
    QList<params_sel> *options;
    int selectedIndex;

public:
    void addOption (QString shownValue, float realValue);
    void setSelected(int index);
    int count();
};

#endif // WIDGETSELECTION_H
