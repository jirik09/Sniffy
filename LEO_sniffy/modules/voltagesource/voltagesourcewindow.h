#ifndef VOLTAGESOURCEWINDOW_H
#define VOLTAGESOURCEWINDOW_H

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

#include "VoltageSourceconfig.h"

namespace Ui {
class VoltageSourceWindow;
}

class VoltageSourceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VoltageSourceWindow(VoltageSourceConfig *config, QWidget *parent = nullptr);
    ~VoltageSourceWindow();

    void restoreGUIAfterStartup();

private:
    Ui::VoltageSourceWindow *ui;
    VoltageSourceConfig *config;
};

#endif // VOLTAGESOURCEWINDOW_H
