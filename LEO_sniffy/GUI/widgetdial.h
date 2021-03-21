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
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);
    void addOption (QString shownValue, QString unit,float realValue);
    int getSelectedIndex() const;
    qreal getRealValue() const;
    void setSelectedIndex(int index, bool silent = false);
    void setColor(QString color);

    int getDefaultIndex() const;
    void setDefaultIndex(int value);

private:
    Ui::WidgetDial *ui;
    QList<params_dial> *options;
    int selectedIndex;
    int defaultIndex = 0;

private slots:
    void plusClicked();
    void minusClicked();
    void valChanged(int in);

signals:
    void valueChanged(float val);
};

#endif // WIDGETDIAL_H
