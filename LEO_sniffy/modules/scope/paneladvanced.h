#ifndef PANELADVANCED_H
#define PANELADVANCED_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgettextinput.h"

class PanelAdvanced  : public QObject
{
public:
    PanelAdvanced(QVBoxLayout *destination, QWidget *parent = nullptr);

    WidgetButtons *resolutionButtons;
    WidgetTextInput *samplingFrequencyInput;
    WidgetTextInput *dataLengthInput;
};

#endif // PANELADVANCED_H
