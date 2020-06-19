#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

#include "GUI/moduledockwidget.h"
#include "GUI/widgetcontrolmodule.h"
#include "GUI/widgetseparator.h"
#include "GUI/widgetfooter.h"
#include "modules/scope/windowscope.h"
#include "modules/counter/windowcounter.h"
#include "GUI/widgettab.h"
#include "device.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void connectDevice(int index);
    void disconnectDevice();


  //  Comms *communication;
    Device *device;

    WidgetSelection *deviceSelection;
    WidgetButtons *deviceConnectButton;

    Ui::MainWindow *ui;

    Device *dev;
    WidgetControlModule *WidgetModule_scope = NULL;
    ModuleDockWidget *dockWidget_scope = NULL;
    WindowScope *scpWindow;

    WidgetControlModule *WidgetFeature_counter = NULL;
    QDockWidget *dockWidget_counter = NULL;
    WindowCounter *cnt;

    WidgetControlModule *WidgetFeature_gen = NULL;

    QScrollArea *scrollAreaSpecification;
    QWidget *WidgetSpecification;
    QVBoxLayout *verticalLayoutSpecification;


    WidgetSeparator *deviceParameters;
    WidgetLabel *labelMCU;
    WidgetLabel *labelCoreFreq;
    WidgetLabel *labelFWVer;
    WidgetLabel *labelRTOSVer;
    WidgetLabel *labelHALVer;


private slots:
   void setMenuSize();
   void deviceConnection(int buttonIndex);
   void updateGUIDeviceList(QList<DeviceDescriptor> deviceList);
   void updateSpecGUI();

   void errorHandler(QByteArray error);

};
#endif // MAINWINDOW_H
