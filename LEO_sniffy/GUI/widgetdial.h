#ifndef WIDGETDIAL_H
#define WIDGETDIAL_H

#include <QWidget>
#include <QList>
#include <QString>

#include "customdial.h"



namespace Ui {
class WidgetDial;
}

typedef struct
{
    QString shownValue;
    QString unit;
    float realValue;
} params_dial;

class WidgetDial : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDial(QWidget *parent = nullptr, QString name = "");
    ~WidgetDial();
    void addOption (QString shownValue, QString unit,float realValue);
    void setSelected(int index);
    void setDialColor(QString &textStyleSheet);
    void setDialButtonsColor(QString &bckgndStyleSheet);
    float getValue();

private:
    Ui::WidgetDial *ui;
    QList<params_dial> *options;
    int selectedIndex;

private slots:
    void plusClicked();
    void minusClicked();
    void valChanged(int in);

signals:
    void valueChanged(float val);
};

#endif // WIDGETDIAL_H
