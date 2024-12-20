#ifndef CUSTOMSETTINGS_H
#define CUSTOMSETTINGS_H

#include <QObject>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QMessageBox>
#include <QDateTime>

#include "graphics/graphics.h"


class CustomSettings : public QObject
{
    Q_OBJECT
public:
    explicit CustomSettings(QObject *parent = nullptr);

    static void loadSettings(QString fileName);
    static void saveSettings();

    static bool askToSaveSession();
    static bool askForSessionRestore(QString device);
    static bool isSessionRestoreRequest();
    static void setNoSessionfound();

    static void setRestoreSession(int value);
    static int getRestoreSession();
    static void setThemeIndex(int value);
    static int getThemeIndex();

    static void addTheme(QString name);
    static void setThemesList(QList<QString> *list);
    static QList<QString> *getThemesList();

    static QString getUserEmail();
    static void setUserEmail(const QString &value);

    static QString getUserPin();
    static void setUserPin(const QString &value);

    static QString getLoginToken();
    static void setLoginToken(const QString &value);

    static QDateTime getTokenValidity();
    static void setTokenValidity(const QDateTime &value);

private:
    static QString settingFile;

    static int restoreSession; //0 no, 1 ask, 2 yes
    static int sessionRestoreAnswer; //-1 not answered
    static int themeIndex;    
    static QList<QString> *themesList;

    static QString userEmail;
    static QString userPin;
    static QString loginToken;
    static QDateTime tokenValidity;

signals:

};

#endif // CUSTOMSETTINGS_H
