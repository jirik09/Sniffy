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
#include "settingsdialog.h"
#include "GUI/widgetfooter.h"
#include "logindialog.h"
#include "GUI/widgetlogininfo.h"

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
    QList<QSharedPointer<AbstractModule>> modulesList;
    QSharedPointer<AbstractModule> deviceModule; // first module (Device)
    QString layoutFile;
    QString configFile;
    QString setingsFile;

    SettingsDialog *sett;
    LoginDialog *logindial;

    bool isLeftMenuNarrow = false; // default to wide menu
    WidgetFooter *footer;
    WidgetLoginInfo *loginInfo;

    void setMenuNarrow();
    void setMenuWide();
    void recoverLeftMenu(bool isWide);

    void setupMainWindowComponents();
    void createModulesWidgets();

    void saveLayout();
    // Update left menu to compact (narrow) or expanded (wide): hide/show texts accordingly
    void updateLeftMenuCompact(bool compact);

private slots:
    void loadLayout(QString deviceName);
    void loadModuleLayoutAndConfigCallback(QString moduleName);
    void setMenuSize(bool isWide);
    void openLoginDialog();
    void updateLoginInfo();
    void openSettingDialog();
};
#endif // MAINWINDOW_H
