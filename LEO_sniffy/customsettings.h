#ifndef CUSTOMSETTINGS_H
#define CUSTOMSETTINGS_H

#include <QObject>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QMessageBox>
#include <QDateTime>

#include "graphics/graphics.h"

class QWidget;


class CustomSettings : public QObject
{
public:
    explicit CustomSettings(QObject *parent = nullptr);

    static void loadSettings(QString fileName);
    static void saveSettings();

    static bool askForSessionRestore(QString device, QWidget *parent = nullptr);
    static bool isSessionRestoreRequest();
    static void setNoSessionfound();

    static void setRestoreSession(int value);
    static int getRestoreSession();
    static void setThemeIndex(int value);
    static int getThemeIndex();

    // Smart session: pre-show geometry restore and skip-geometry on auto-load
    static bool getSmartSessionLayoutGeometry();
    static void setSmartSessionLayoutGeometry(bool value);

    static void addTheme(QString name);
    static void setThemesList(QList<QString> *list);
    static QList<QString> *getThemesList();

    static QString getUserEmail();
    static void setUserEmail(const QString &value);

    static QString getUserPin();
    static void setUserPin(const QString &value);

    static QByteArray getLoginToken();
    static void setLoginToken(const QByteArray &value);

    static QDateTime getTokenValidity();
    static void setTokenValidity(const QDateTime &value);

    static QString getLastLoginFailure();
    static void setLastLoginFailure(const QString &value);

    // Authentication helpers
    static bool hasValidLogin();

private:
    static QString settingFile;

    static int restoreSession; //0 no, 1 ask, 2 yes
    static int sessionRestoreAnswer; //-1 not answered
    static int themeIndex;    
    static QList<QString> *themesList;
    static bool smartSessionLayoutGeometry; // default true

    static QString userEmail;
    static QString userPin;
    static QByteArray loginToken;
    static QDateTime tokenValidity;
    static QString lastLoginFailureReason;

signals:

};

#endif // CUSTOMSETTINGS_H
