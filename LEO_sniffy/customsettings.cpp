#include "customsettings.h"
#include "qdebug.h"
#include <QApplication>
#include <QWidget>

CustomSettings::CustomSettings(QObject *parent) : QObject(parent)
{
}

QString CustomSettings::settingFile;
int CustomSettings::restoreSession;
int CustomSettings::sessionRestoreAnswer;
int CustomSettings::themeIndex;
QList<QString> *CustomSettings::themesList = nullptr;
bool CustomSettings::smartSessionLayoutGeometry = false;
QString CustomSettings::userEmail;

QByteArray CustomSettings::loginToken;
QDateTime CustomSettings::tokenValidity;
QDate CustomSettings::tokenGeneratedDate;
QString CustomSettings::lastLoginFailureReason;

void CustomSettings::loadSettings(QString fileName)
{
    settingFile = fileName;
    QFile file(settingFile);
    if(file.exists()){
        QSettings settings(settingFile, QSettings::IniFormat);

        restoreSession = settings.value("restoreSession").toInt();
        // Prefer Dawn as default if key is missing
        themeIndex = settings.contains("theme") ? settings.value("theme").toInt() : 2;

    userEmail = settings.value("email").toString();
    // Default OFF if key missing
    smartSessionLayoutGeometry = settings.contains("smartSessionLayoutGeometry") ?
                     settings.value("smartSessionLayoutGeometry").toBool() : false;
        if(userEmail == "Unknown user"){
            // Treat legacy sentinel as empty so UI shows placeholder instead of editable text
            userEmail.clear();
        }
        loginToken = settings.value("token").toByteArray();

        tokenValidity = settings.value("validity").toDateTime();
        tokenGeneratedDate = settings.value("tokenGeneratedDate").toDate();
        lastLoginFailureReason = settings.value("last_login_failure").toString();
    }else{ // set default values if settings file doesnt exist
        restoreSession = 0; // default: No session restore
        // Default theme: Dawn
        themeIndex = 2;
        smartSessionLayoutGeometry = false;
        userEmail = "Unknown user";
        loginToken = "none";
        tokenValidity = QDateTime(QDate(2000,1,1),QTime(0,0));
        tokenGeneratedDate = QDate();
        lastLoginFailureReason = "";
    }
    sessionRestoreAnswer = -1;
}

void CustomSettings::saveSettings()
{
    QSettings settings(settingFile, QSettings::IniFormat);

    settings.setValue("restoreSession", restoreSession);
    settings.setValue("theme", themeIndex);
    settings.setValue("smartSessionLayoutGeometry", smartSessionLayoutGeometry);

    // Never persist legacy sentinel; store empty instead
    if(userEmail == "Unknown user"){
        settings.setValue("email", "");
    }else{
        settings.setValue("email", userEmail);
    }
    // Safeguard: do not overwrite an existing valid token with an empty placeholder inadvertently
    if(loginToken.isEmpty()){
        QByteArray existing = settings.value("token").toByteArray();
        if(!existing.isEmpty()){
            loginToken = existing; // preserve previous token
        }
    }
    settings.setValue("token", loginToken);
    settings.setValue("validity", tokenValidity);
    settings.setValue("tokenGeneratedDate", tokenGeneratedDate);
    settings.setValue("last_login_failure", lastLoginFailureReason);

}

void CustomSettings::setRestoreSession(int value)
{
    restoreSession = value;
}

bool CustomSettings::isSessionRestoreRequest()
{
    if(sessionRestoreAnswer == 1) return true;
    return false;
}

void CustomSettings::setNoSessionfound()
{
    sessionRestoreAnswer = 0;
}

void CustomSettings::setThemeIndex(int value)
{
    themeIndex = value;
}

int CustomSettings::getThemeIndex()
{
    return themeIndex;
}

bool CustomSettings::getSmartSessionLayoutGeometry()
{
    return smartSessionLayoutGeometry;
}

void CustomSettings::setSmartSessionLayoutGeometry(bool value)
{
    smartSessionLayoutGeometry = value;
}

void CustomSettings::addTheme(QString name)
{
    themesList->append(name);
}

void CustomSettings::setThemesList(QList<QString> *list)
{
    themesList = list;
}

QList<QString>* CustomSettings::getThemesList()
{
    return themesList;
}

QString CustomSettings::getUserEmail()
{
    return userEmail;
}

void CustomSettings::setUserEmail(const QString &value)
{
    userEmail = value;
}

QByteArray CustomSettings::getLoginToken()
{
    return loginToken;
}

void CustomSettings::setLoginToken(const QByteArray &value)
{
    loginToken = value;
}

QDateTime CustomSettings::getTokenValidity()
{
    return tokenValidity;
}

void CustomSettings::setTokenValidity(const QDateTime &value)
{
    tokenValidity = value;
}

QDate CustomSettings::getTokenGeneratedDate()
{
    return tokenGeneratedDate;
}

void CustomSettings::setTokenGeneratedDate(const QDate &value)
{
    tokenGeneratedDate = value;
}

QString CustomSettings::getLastLoginFailure()
{
    return lastLoginFailureReason;
}

bool CustomSettings::hasValidLogin()
{
    // Valid if token not 'none', validity in future, and last failure empty
    if(loginToken.isEmpty() || loginToken == QByteArray("none")) return false;
    if(!tokenValidity.isValid()) return false;
    if(tokenValidity < QDateTime::currentDateTime()) return false;
    return true;
}

void CustomSettings::setLastLoginFailure(const QString &value)
{
    lastLoginFailureReason = value;
}

int CustomSettings::getRestoreSession()
{
    return restoreSession;
}

bool CustomSettings::askForSessionRestore(QString device, QWidget *parent)
{
    if(restoreSession == 0){
        sessionRestoreAnswer = 0;
        return false;
    }else if (restoreSession == 2){
        sessionRestoreAnswer = 1;
        return true;
    }

    QWidget *dlgParent = parent ? parent : QApplication::activeWindow();
    QMessageBox msgBox(dlgParent);

    msgBox.setWindowTitle("Session restore");
    msgBox.setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));
    msgBox.setStyleSheet("QPushButton{width:50px;}");
    msgBox.setText("Do you want to restore previous session");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setInformativeText("for: "+device+" ");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    switch (ret) {
    case QMessageBox::Yes:
        sessionRestoreAnswer = 1;
        return true;
        break;
    case QMessageBox::No:
    case QMessageBox::Cancel:
        sessionRestoreAnswer = 0;
        return false;
        break;
    }
    sessionRestoreAnswer = 0;
    return false;
}
