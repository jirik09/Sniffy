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

class ToastWidget; // Forward declaration

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
    // Restores layout; optionally skip main window geometry (position/size)
    void loadLayoutSessionFromFile(bool restoreMainGeometry = true);
    void loadModulesSessionFromFile(const QString &moduleName);
    // Update left menu to compact (narrow) or expanded (wide): hide/show texts accordingly
    void updateLeftMenuCompact(bool compact);

    // On startup: restore only main window geometry from the newest session (if any)
    void restoreInitialGeometryFromNewestSession();

    // Smart session: keep position but update size from session, expanding symmetrically and fitting screen
    void applySmartSessionSizeOnly();

    QList<ToastWidget*> m_toasts;

private slots:
    void onSettingsSaveSessionRequested();
    void onExitSaveSessionRequested();
    void onSettingsLoadSessionRequested();
    void onOpenLoadSessionRequested(QString deviceName);
    void setMenuSize(bool isWide);
    void openLoginDialog();
    void updateLoginInfo(const QDateTime &validity, const QByteArray &token, bool authenticationSentManual);
    void onLoginInfoChanged(); // Update login info widget when logged out
    void openSettingDialog();
    // Show a small popup/toast in the bottom-left corner of the app window
    void showBottomLeftPopup(const QString &text);
    //void onModuleControlShown();

    void repositionToasts();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
};
#endif // MAINWINDOW_H
