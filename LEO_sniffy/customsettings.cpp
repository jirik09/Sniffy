#include "customsettings.h"
#include "qdebug.h"

CustomSettings::CustomSettings(QObject *parent) : QObject(parent)
{
}

QString CustomSettings::settingFile;
int CustomSettings::restoreSession;
int CustomSettings::sessionRestoreAnswer;
int CustomSettings::themeIndex;
QList<QString> *CustomSettings::themesList = nullptr;
QString CustomSettings::userEmail;
QString CustomSettings::userPin;
QByteArray CustomSettings::loginToken;
QDateTime CustomSettings::tokenValidity;
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
        if(userEmail == "Unknown user"){
            // Treat legacy sentinel as empty so UI shows placeholder instead of editable text
            userEmail.clear();
        }
        userPin = settings.value("pin").toString();
        loginToken = QByteArray::fromHex(settings.value("token").toByteArray());
        // Previously this used QByteArray::fromHex which corrupted an already ASCII token on reload.
        // Store and load the token verbatim to keep what the server provided.
        // loginToken = settings.value("token").toByteArray();
        tokenValidity = settings.value("validity").toDateTime();
        lastLoginFailureReason = settings.value("last_login_failure").toString();
    }else{ // set default values if settings file doesnt exist
        restoreSession = 0; // default: No session restore
        // Default theme: Dawn
        themeIndex = 2;
        userEmail = "Unknown user";
        userPin = "0000";
        loginToken = "none";
        tokenValidity = QDateTime(QDate(2000,1,1),QTime(0,0));
        lastLoginFailureReason = "";
    }
    sessionRestoreAnswer = -1;
}

void CustomSettings::saveSettings()
{
    QSettings settings(settingFile, QSettings::IniFormat);

    settings.setValue("restoreSession", restoreSession);
    settings.setValue("theme", themeIndex);

    // Never persist legacy sentinel; store empty instead
    if(userEmail == "Unknown user"){
        settings.setValue("email", "");
    }else{
        settings.setValue("email", userEmail);
    }
    settings.setValue("pin", userPin);
    // Safeguard: do not overwrite an existing valid token with an empty placeholder inadvertently
    if(loginToken.isEmpty()){
        QByteArray existing = settings.value("token").toByteArray();
        if(!existing.isEmpty()){
            loginToken = existing; // preserve previous token
        }
    }
    settings.setValue("token", loginToken);
    settings.setValue("validity", tokenValidity);
    settings.setValue("last_login_failure", lastLoginFailureReason);

}

bool CustomSettings::askToSaveSession()
{
    if(restoreSession == 2) return true;
    if(restoreSession == 0) return false;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Save session");
    msgBox.setWindowIcon(QIcon(":/graphics/graphics/logo_color.png"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStyleSheet("QPushButton{width:50px;}");
    msgBox.setText("Do you want to save current session?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    switch (ret) {
    case QMessageBox::Yes:
        return true;
        break;
    case QMessageBox::No:
    case QMessageBox::Cancel:
        return false;
        break;
    }
    return false;
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

QString CustomSettings::getUserPin()
{
    return userPin;
}

void CustomSettings::setUserPin(const QString &value)
{
    userPin = value;
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

bool CustomSettings::askForSessionRestore(QString device)
{
    if(restoreSession == 0){
        sessionRestoreAnswer = 0;
        return false;
    }else if (restoreSession == 2){
        sessionRestoreAnswer = 1;
        return true;
    }

    QMessageBox msgBox;

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
