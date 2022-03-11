#ifndef PATTERNGENERATORWINDOW_H
#define PATTERNGENERATORWINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QDebug>
#include <QVBoxLayout>
#include <QFileDialog>

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
#include "../../GUI/qcustomplot.h"

#include "patterngeneratorconfig.h"
#include "patterngeneratorspec.h"
#include "patterngeneratorsettings.h"
#include "patterngeneratorpatterns.h"
#include "patterngeneratorpainter.h"

namespace Ui {
class PatternGeneratorWindow;
}

class PatternGeneratorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PatternGeneratorWindow(PatternGeneratorConfig *config, QWidget *parent = nullptr);
    ~PatternGeneratorWindow();

    PatternGeneratorSettings *settings;    
    PatternGeneratorPatterns *patterns;

    void restoreGUIAfterStartup();
    void setSpecification(PatternGeneratorSpec* spec);    

    void setProgress(int percent);
    void setGenerateButton(QString text, QString color);
    void setGeneratorState(bool onClick);

    QList<patttype> *getPatternData();

private:
    Ui::PatternGeneratorWindow *ui;
    PatternGeneratorConfig *config;
    PatternGeneratorSpec *spec;    
    PatternGeneratorPainter *painter;

//    QCustomPlot *plot;
//    QCPGraph *graph;
//    widgetChart *chart;

    QList<patttype> *patternData;

private slots:
    void patternSelectionChangedCallback(int index);

    void runGeneratorCallback();
    void openFileCallback();
    void resetPatternCallback();   

    void freqChangedDialsCallback(float val);
    void freqChangedCombosCallback(int index, float realVal);
    void dataLenChangedDialsCallback(float val);

    void quadratureSequenceChangedCallback(int index);

    void chartEditDataOnLeftClickCallback(QGraphicsSceneMouseEvent* event);

signals:
    void runGenerator();
    void stopGenerator();
};

#endif // PATTERNGENERATORWINDOW_H
