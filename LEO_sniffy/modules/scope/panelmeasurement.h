#ifndef PANELMEASUREMENT_H
#define PANELMEASUREMENT_H

#include <QObject>
#include <QVBoxLayout>

#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetswitch.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"

class PanelMeasurement: public QObject
{
    Q_OBJECT
public:
    explicit PanelMeasurement(QVBoxLayout *destination, QWidget *parent = nullptr);
};

#endif // PANELMEASUREMENT_H
