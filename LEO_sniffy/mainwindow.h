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
#include <QTimer>

#include "devicemediator.h"
#include "settingsdialog.h"
#include "GUI/widgetfooter.h"
#include "logindialog.h"
#include "GUI/widgetlogininfo.h"
#include "authenticator.h"

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
    Authenticator *authenticator; // Shared authenticator for login and token refresh

    bool isLeftMenuNarrow = false; // default to wide menu
    WidgetFooter *footer;
    WidgetLoginInfo *loginInfo;
    QJsonObject sessionRestoreData;


    void setMenuNarrow();
    void setMenuWide();
    void recoverLeftMenu(bool isWide);
    void enforceLeftMenuWidth();
    void enforceLeftMenuWidthSoon();

    // Two fixed widths for the left panel (menu)
    static constexpr int LeftMenuNarrowWidth = 90;   // collapsed
    static constexpr int LeftMenuWideWidth   = 250;  // expanded

    void setupMainWindowComponents();
    void createModulesWidgets();

    void saveSessionToFile(const QString &filePath, bool silent);
    bool loadSessionJSONFile(const QString &filePath);
    void loadLayoutSessionFromFile();
    void loadModulesSessionFromFile(const QString &moduleName);
    // Update left menu to compact (narrow) or expanded (wide): hide/show texts accordingly
    void updateLeftMenuCompact(bool compact);


private slots:
    void onSettingsSaveSessionRequested();
    void onExitSaveSessionRequested();
    void onSettingsLoadSessionRequested();
    void onOpenLoadSessionRequested(QString deviceName);
    void setMenuSize(bool isWide);
    void openLoginDialog();
    void updateLoginInfo();
    void openSettingDialog();
    //void onModuleControlShown();

protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H
