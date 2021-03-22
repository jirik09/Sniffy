#ifndef WIDGETDIALRANGE_H
#define WIDGETDIALRANGE_H

#include <QWidget>
#include <QList>
#include <QString>
#include "math.h"

#include "customdial.h"
#include "../graphics/colors.h"


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
    QByteArray saveGeometry();
    void restoreGeometry(QByteArray geom);
    void addOption (QString unitString,float mult);
    void setRange(float min, float max, QString unitString, float buttonStep = 1, float precision = 1, float defaultValue = 0, bool log = false);
    void updateRange(float min, float max, bool silent = false);
    void enableFineMousePrecision();
    void setRealValue(float value, bool silent = false);
    void hideUnitSelection(void);
    void setColor(QString color);
    void setDialColor(QString &styleSheet);
    void setDialButtonsColor(QString &bckgndStyleSheet);
    float getDefaultRealValue() const;
    float getRealValue() const;

private:
    int optionalEmitParam;
    Ui::WidgetDialRange *ui;
    QList<params_unit> *units;
    int dialMaxValue = 500;
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
