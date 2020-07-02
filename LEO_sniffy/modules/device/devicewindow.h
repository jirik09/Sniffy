#ifndef WINDOWSCAN_H
#define WINDOWSCAN_H

#include <QScrollArea>
#include <QVBoxLayout>

#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetseparator.h"

#include "devicespec.h"

namespace Ui {
class DeviceWindow;
}

class DeviceWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceWindow(QWidget *parent = nullptr);
    ~DeviceWindow();

    void showSpecification(DeviceSpec *spec);
    void hideSpecification();



    WidgetSelection *deviceSelection;
    WidgetButtons *deviceConnectButton;

    QWidget *WidgetSpecification;
    QVBoxLayout *verticalLayoutSpecification;
private:

    QSpacerItem *verticalSpacer;
    QScrollArea *scrollAreaSpecification;
    WidgetSeparator *deviceParameters;
    WidgetLabel *labelMCU;
    WidgetLabel *labelCoreFreq;
    WidgetLabel *labelFWVer;
    WidgetLabel *labelRTOSVer;
    WidgetLabel *labelHALVer;


    Ui::DeviceWindow *ui;

signals:


};

#endif // WINDOWSCAN_H
