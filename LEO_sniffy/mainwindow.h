#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QMessageBox>

#include "GUI/widgetfeature.h"
#include "GUI/widgetseparator.h"
#include "GUI/widgetfooter.h"
#include "GUI/windowscope.h"
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
    Ui::MainWindow *ui;

    Device *dev;
    WidgetFeature *WidgetFeature_scope = NULL;
    QDockWidget *dockWidget_scope = NULL;
    WindowScope *scp;

    WidgetFeature *WidgetFeature_gen = NULL;

    Device *device;

    QScrollArea *scrollAreaSpecification;
    QWidget *WidgetSpecification;
    QVBoxLayout *verticalLayoutSpecification;

private slots:
   void setMenuSize();
   void openScope();

};
#endif // MAINWINDOW_H
