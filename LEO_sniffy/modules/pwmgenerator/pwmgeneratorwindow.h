#ifndef PWMGENERATORWINDOW_H
#define PWMGENERATORWINDOW_H

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

#include "PWMGeneratorconfig.h"

namespace Ui {
class PWMGeneratorWindow;
}

class PWMGeneratorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PWMGeneratorWindow(PWMGeneratorConfig *config, QWidget *parent = nullptr);
    ~PWMGeneratorWindow();

    void restoreGUIAfterStartup();

private:
    Ui::PWMGeneratorWindow *ui;
    PWMGeneratorConfig *config;
};

#endif // PWMGENERATORWINDOW_H
