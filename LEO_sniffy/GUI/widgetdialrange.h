#ifndef WIDGETDIALRANGE_H
#define WIDGETDIALRANGE_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QtMath>
#include "customdial.h"
#include "../graphics/graphics.h"
#include "../modules/numberparser.h"

#define DIAL_DEFAULT_RANGE 700

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
    explicit WidgetDialRange(QWidget *parent = nullptr, QString name = "", int optionalEmitParam = 0);
    ~WidgetDialRange();
    void setName(QString name);
    void setAdditionalLabelText(QString text);
    void setAdditionalLabelColor(QString color);
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);
    void addOption (QString unitString,float mult);
    void setRange(float min, float max, QString unitString, float buttonStep = 1, float precision = 1, float defaultValue = 0, bool log = false, int numOfDecimals = 2, bool silent = false);
    void updateBaseUnit(QString unit);
    void updateRange(float min, float max, bool silent = false);
    void enableFineMousePrecision();
    void setRealValue(float value, bool silent = false);
    void hideUnitSelection(void);
    void setColor(QString color);
    float getDefaultRealValue() const;
    float getRealValue() const;
    void disable(bool disable);    

    void setNumOfDecimals(int value);
    // Added getters for current numeric range to support accurate percent mapping
    float getRangeMin() const { return rangeMin; }
    float getRangeMax() const { return rangeMax; }

private:
    int optionalEmitParam;
    Ui::WidgetDialRange *ui;
    QList<params_unit> units; // direct ownership (no heap allocation)
    int dialMaxValue = DIAL_DEFAULT_RANGE;
    float realValue;    
    float defaultRealValue = 0;
    float labelValue;

    float unitMult;

    float rangeMax;
    float rangeMin;
    float rangePrecision;
    float dialStep;
    float buttonStep;
    QString unitString;

    bool logaritmic;
    int numOfDecimals = 2;
    float logOffset = 0;
    float logGain = 0;

    void updateControls(int except, bool silent = false);
    float getRealValueFromDial(int in);
    int getValueForDial(float real);

private slots:
    void plusClicked();
    void minusClicked();
    void dialValueChanged(int in);
    void unitChanged(int in);
    void textEditChanged(const QString & text);
    void textEditFinished();

signals:
    void valueChanged(float val,int optionalEmitParam = 0);
};

#endif // WIDGETDIALRANGE_H
