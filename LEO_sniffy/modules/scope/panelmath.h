#ifndef PANELMATH_H
#define PANELMATH_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgettextinput.h"

#include "fftengine.h"

class PanelMath : public QObject
{
    Q_OBJECT
public:
    explicit PanelMath(QVBoxLayout *destination, QWidget *parent = nullptr);
    void symbolicError(int errorPosition);

signals:
    void expressionChanged(QString exp);
    void fftChanged(int length, FFTWindow window, FFTType type, int channelIndex);

public:
    WidgetButtons *mathType;

    WidgetButtons *btnChannelASel;
    WidgetButtons *btnChannelBSel;
    WidgetSelection *operatorSel;

    WidgetLabel *symbolicTitle;
    WidgetLabel *symbolicDesc;
    WidgetLabel *symbolicExample;
    QString errorExp = "";
    WidgetTextInput *symbolicExpression;

    WidgetButtons *btnChannelFFTSel;

    int previousMathType = 0;

public slots:
    void typeChanged(int index);
private slots:
    void symbolicExpressionCallback(QString exp);
    void fftCallback();

private:
    void hideAll();
};

#endif // PANELMATH_H
