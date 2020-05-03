#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include "widgetfeature.h"
#include "widgetseparator.h"
#include "widgetfooter.h"
#include "windowscope.h"

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

    WidgetFeature *WidgetFeature_scope = NULL;
    QDockWidget *dockWidget_scope = NULL;
    WindowScope *scp;

    WidgetFeature *WidgetFeature_gen = NULL;

private slots:
   void setMenuSize();
   void openScope();
};
#endif // MAINWINDOW_H
