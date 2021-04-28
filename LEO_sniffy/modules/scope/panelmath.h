#ifndef PANELMATH_H
#define PANELMATH_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgettextinput.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"

#include "../labelformator.h"

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
    void fftchartChanged(qreal scale, qreal shift, bool isLog = false);


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
    WidgetSelection *selFFTWindow;
    WidgetButtons *swFFTType;
    WidgetSelection *selFFTLength;
    WidgetDial *dialFFTVertical;
   // WidgetDial *dialFFTHorizontal;
    WidgetDialRange *dialFFTShift;


    int previousMathType = 0;

public slots:
    void typeChanged(int index);
private slots:
    void symbolicExpressionCallback(QString exp);
    void fftCallback();
    void fftTypeCallback();
    void fftChartCallback();

private:
    void hideAll();
    void fillVerticalDials();
    void fillHorizontalDials();
};

#endif // PANELMATH_H
