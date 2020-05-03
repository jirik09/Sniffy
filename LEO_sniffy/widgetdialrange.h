#ifndef WIDGETDIALRANGE_H
#define WIDGETDIALRANGE_H

#include <QWidget>
#include <QList>
#include <QString>
#include "math.h"

#include "customdial.h"



namespace Ui {
class WidgetDialRange;
}

typedef struct
{
    QString unit;
    float mult;
} params_unit;

class WidgetDialRange : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDialRange(QWidget *parent = nullptr, QString name = "");
    ~WidgetDialRange();
    void addOption (QString unitString,float mult);
    void setSelected(int index);
    void setRange(float min, float max, QString unitString, float buttonStep = 1, float precision = 1, float defaultValue = 0, bool log = false);
    void hideUnitSelection(void);

private:
    Ui::WidgetDialRange *ui;
    QList<params_unit> *units;
    const int dialMaxValue = 500;
    float realValue;
    float labelValue;

    float unitMult;

    float rangeMax;
    float rangeMin;
    float rangePrecision;
    float dialStep;
    float buttonStep;
    QString unitString;

    bool logaritmic;
    float logOffset = 0;
    float logGain = 0;



    CustomDial *dial;

    void updateControls(int except);
    float getRealValueFromDial(int in);
    int getValueForDial(float real);

private slots:
    void plusClicked();
    void minusClicked();
    void dialValueChanged(int in);
    void unitChanged(int in);
    void textEditChanged(const QString & text);
    void textEditFinished();

};

#endif // WIDGETDIALRANGE_H
