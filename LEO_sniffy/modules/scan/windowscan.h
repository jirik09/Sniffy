#ifndef WINDOWSCAN_H
#define WINDOWSCAN_H

#include <QScrollArea>

#include "modules/scope/scope.h"
#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetseparator.h"

namespace Ui {
class WindowScan;
}

class WindowScan : public QWidget
{
    Q_OBJECT
public:
    explicit WindowScan(QWidget *parent = nullptr);
    ~WindowScan();

    ModuleDockWidget *dockWidgetWindow;

    QSpacerItem *verticalSpacer;
    QScrollArea *scrollAreaSpecification;

    WidgetSelection *deviceSelection;
    WidgetButtons *deviceConnectButton;

    QWidget *WidgetSpecification;
    QVBoxLayout *verticalLayoutSpecification;

    WidgetSeparator *deviceParameters;
    WidgetLabel *labelMCU;
    WidgetLabel *labelCoreFreq;
    WidgetLabel *labelFWVer;
    WidgetLabel *labelRTOSVer;
    WidgetLabel *labelHALVer;

private:
    Ui::WindowScan *ui;

signals:


};

#endif // WINDOWSCAN_H
