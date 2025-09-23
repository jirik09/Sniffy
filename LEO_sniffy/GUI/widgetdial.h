#ifndef WIDGETDIAL_H
#define WIDGETDIAL_H

#include <QWidget>
#include <QList>
#include <QString>

#include "customdial.h"
#include "../graphics/graphics.h"


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
    explicit WidgetDial(QWidget *parent = nullptr, QString name = "", int optionalEmitParam = 0);
    ~WidgetDial();
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);
    void addOption (QString shownValue, QString unit,float realValue);
    void clearOptions();
    int getSelectedIndex() const;
    qreal getRealValue() const;
    void setSelectedIndex(int index, bool silent = false);
    void setColor(QString color);

    int getDefaultIndex() const;
    void setDefaultIndex(int value);

private:
    Ui::WidgetDial *ui;
    int optionalEmitParam = 0;
    QList<params_dial> options; // value container (owned directly)
    int selectedIndex;
    int defaultIndex = 0;

private slots:
    void plusClicked();
    void minusClicked();
    void valChanged(int in);

signals:
    void valueChanged(float val, int optionalEmitParam = 0);
};

#endif // WIDGETDIAL_H
