#ifndef TEMPLATEMODULEWINDOW_H
#define TEMPLATEMODULEWINDOW_H

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

#include "voltmeterconfig.h"

namespace Ui {
class VoltmeterWindow;
}

class VoltmeterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VoltmeterWindow(VoltmeterConfig *config, QWidget *parent = nullptr);
    ~VoltmeterWindow();

    void restoreGUIAfterStartup();

private:
    Ui::VoltmeterWindow *ui;
    VoltmeterConfig *config;
};

#endif // TEMPLATEMODULEWINDOW_H
