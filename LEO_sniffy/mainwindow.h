#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>

#include "devicemediator.h"
#include "GUI/widgetfooter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent (QCloseEvent *event);

private:    
    Ui::MainWindow *ui;
    DeviceMediator *deviceMediator;
    QPropertyAnimation *animation;
    QString layoutFile;
    QString configFile;
    QString setingsFile;

    void setMenuNarrow();
    void setMenuWide();

    void setupMainWindowComponents();
    void createModulesWidgets();

    void saveLayout();

private slots:
    void loadMainLayout(QString deviceName);
    void attachWidgets();
    void setMenuSize(bool isWide);
};
#endif // MAINWINDOW_H
