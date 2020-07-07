#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "devicemediator.h"

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
    DeviceMediator *deviceMediator;
    void setMenuNarrow();
    void setMenuWide();

    void setupMainWindowComponents();
    void createModulesWidgets();

private slots:
   void setMenuSize(bool isWide);
};
#endif // MAINWINDOW_H
