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
    QString layoutFile;
    QString configFile;
    QString setingsFile;

    SettingsDialog *sett;
    bool isLeftMenuNarrow;
    WidgetFooter *footer;

    void setMenuNarrow();
    void setMenuWide();
    void recoverLeftMenu(bool isWide);

    void setupMainWindowComponents();
    void createModulesWidgets();

    void saveLayout();

private slots:
    void loadLayout(QString deviceName);
    void loadModuleLayoutAndConfigCallback(QString moduleName);
    void setMenuSize(bool isWide);
    void openSettingDialog();
};
#endif // MAINWINDOW_H
