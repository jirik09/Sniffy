#ifndef ARBGENERATORWINDOW_H
#define ARBGENERATORWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QDebug>
#include <QVBoxLayout>

#include "../../GUI/widgetcontrolmodule.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdial.h"
#include "../../GUI/widgetdialrange.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetchart.h"
#include "../../GUI/widgettab.h"
#include "../../GUI/widgetlabelarea.h"
#include "../../GUI/widgettextinput.h"

#include "arbgeneratorconfig.h"
#include "arbgenpanelsettings.h"
#include "arbgeneratorspec.h"
#include "signalcreator.h"

namespace Ui {
class ArbGeneratorWindow;
}

class ArbGeneratorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ArbGeneratorWindow(ArbGeneratorConfig *config, QWidget *parent = nullptr);
    ~ArbGeneratorWindow();

    void restoreGUIAfterStartup();
    void setSpecification(ArbGeneratorSpec* spec);

private:

    Ui::ArbGeneratorWindow *ui;
    ArbGeneratorConfig *config;
    ArbGeneratorSpec *spec;
    ArbGenPanelSettings *setting;

    widgetChart *chart;
    QVector<QVector<QPointF>> *generatorChartData;
    QList<QList<int>> *generatorDACData;

private slots:
    void generateSignalCallback();

};

#endif // ARBGENERATORWINDOW_H
