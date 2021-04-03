#ifndef WINDOWSCAN_H
#define WINDOWSCAN_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QFileInfo>

#include "../../GUI/widgetselection.h"
#include "../../GUI/widgetbuttons.h"
#include "../../GUI/widgetlabel.h"
#include "../../GUI/widgetseparator.h"
#include "../../GUI/widgetdesciptionexpand.h"

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

private:
    QWidget *WidgetSpecification;
    QVBoxLayout *verticalLayoutSpecification;
    QSpacerItem *verticalSpacer;
    QScrollArea *scrollAreaSpecification;
   // WidgetSeparator *deviceParameters;

    QList<WidgetDesciptionExpand*> *modulesDescriptions;
    WidgetDesciptionExpand *devDesc;
   /*WidgetLabel *labelMCU;
    WidgetLabel *labelCoreFreq;
    WidgetLabel *labelFWVer;
    WidgetLabel *labelRTOSVer;
    WidgetLabel *labelHALVer;
    WidgetLabel *labelMCU_ID;*/
    Ui::DeviceWindow *ui;
};

#endif // WINDOWSCAN_H
