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

    void addModuleDescription(QString name, QList<QString> labels, QList<QString> values);
    void clearModuleDescriptions();

    WidgetSelection *deviceSelection;
    WidgetButtons *deviceConnectButton;

private:
    QWidget *WidgetSpecification;
    QVBoxLayout *verticalLayoutSpecification;
    QSpacerItem *verticalSpacer;
    QScrollArea *scrollAreaSpecification;

    QVBoxLayout *descriptorsLayout;
    QList<WidgetDesciptionExpand*> *modulesDescriptions;
    WidgetDesciptionExpand *devDesc;

    Ui::DeviceWindow *ui;
};

#endif // WINDOWSCAN_H
