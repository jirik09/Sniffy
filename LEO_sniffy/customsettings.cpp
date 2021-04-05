#include "customsettings.h"

CustomSettings::CustomSettings(QObject *parent) : QObject(parent)
{
}

QString CustomSettings::settingFile;
int CustomSettings::restoreSession;
int CustomSettings::sessionRestoreAnswer;
int CustomSettings::themeIndex;
QList<QString> CustomSettings::themesList;

void CustomSettings::loadSettings(QString fileName)
{
    settingFile = fileName;
    QFile file(settingFile);
    if(file.exists()){
        QSettings settings(settingFile, QSettings::IniFormat);

        restoreSession = settings.value("restoreSession").toInt();
        themeIndex = settings.value("theme").toInt();

    }else{ // set dafeulf values if settings file doesnt exist
        restoreSession = 1;
        themeIndex = 0;

    }
    sessionRestoreAnswer = -1;
}

void CustomSettings::saveSettings()
{
    QSettings settings(settingFile, QSettings::IniFormat);

    settings.setValue("restoreSession", restoreSession);
    settings.setValue("theme", themeIndex);
}

bool CustomSettings::askToSaveSession()
{
    if(restoreSession == 2) return true;
    if(restoreSession == 0) return false;

    QMessageBox msgBox;
    msgBox.setText("Save session");
    msgBox.setInformativeText("Do you want to save current session?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
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
    themesList.append(name);
}

QList<QString> CustomSettings::getThemesList()
{
    return themesList;
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
    msgBox.setText("Session restore");
    msgBox.setInformativeText("Session for "+device+" found. Do you want to restore?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
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
