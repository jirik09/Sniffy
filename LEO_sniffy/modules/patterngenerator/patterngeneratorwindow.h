#ifndef PATTERNGENERATORWINDOW_H
#define PATTERNGENERATORWINDOW_H

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

#include "patterngeneratorconfig.h"
#include "patterngeneratorspec.h"
#include "patterngeneratorsettings.h"

namespace Ui {
class PatternGeneratorWindow;
}

class PatternGeneratorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PatternGeneratorWindow(PatternGeneratorConfig *config, QWidget *parent = nullptr);
    ~PatternGeneratorWindow();

    void restoreGUIAfterStartup();
    void setSpecification(PatternGeneratorSpec* spec);

private:
    Ui::PatternGeneratorWindow *ui;
    PatternGeneratorConfig *config;
    PatternGeneratorSpec *spec;
    PatternGeneratorSettings *settings;

    widgetChart *chart;
    QVector<QVector<QPointF>> *chartData;
};

#endif // PATTERNGENERATORWINDOW_H
