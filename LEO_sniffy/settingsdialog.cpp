#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");

    QVBoxLayout *buttons = new QVBoxLayout();
    buttons->setSpacing(2);
    buttons->setContentsMargins(5,5,5,5);

    WidgetSeparator *sep1 = new WidgetSeparator(this,"General settings");
    buttons->addWidget(sep1);
    buttonsRestoreSession = new WidgetButtons(this,3,ButtonTypes::RADIO,"Restore session after startup",0);
    buttonsRestoreSession->setText("   No   ",0);
    buttonsRestoreSession->setText("Always ask",1);
    buttonsRestoreSession->setText("   Yes   ",2);
    buttons->addWidget(buttonsRestoreSession);

    selTheme = new WidgetSelection(this,"Color scheme");
    selTheme->addOption("Dark",0);
    selTheme->addOption("Light",1);
    selTheme->addOption("Vintage",2);
    selTheme->addOption("Colorful",3);
    selTheme->addOption("Honzuv mega layout",4);
    selTheme->setSelected(0);
    buttons->addWidget(selTheme);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    buttons->addItem(verticalSpacer);

    WidgetSeparator *sep2 = new WidgetSeparator(this);
    buttons->addWidget(sep2);

    buttonsDone = new WidgetButtons(this,2);
    buttonsDone->setText("  Save  ",0);
    buttonsDone->setText(" Cancel ",1);
    buttons->addWidget(buttonsDone);

    ui->widget->setLayout(buttons);

    settingFile = QApplication::applicationDirPath() + "/settings.ini";
    loadSettings();

    buttonsRestoreSession->setChecked(true,restoreSession);
    selTheme->setSelected(themeIndex);

    connect(buttonsDone,&WidgetButtons::clicked,this,&SettingsDialog::closeDialog);
    connect(selTheme,&WidgetSelection::selectedIndexChanged,this,&SettingsDialog::restartWarning);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::open()
{
    loadSettings();
    this->show();
    this->raise();
    this->activateWindow();
}

int SettingsDialog::getThemeIndex() const
{
    return themeIndex;
}

bool SettingsDialog::IsSessionRestoreRequest() const
{
    if(restoreSession == 0)
        return false;
    if(restoreSession == 2)
        return true;
    if(restoreSession == 1){
        switch (sessionRestoreAnswer) {
        case -1:
            //qDebug () << "ERROR Modules doesn't know whether restore or not"; //Device module actually ask before
            return false;
            break;
        case 0:
            return false;
            break;
        case 1:
            return true;
            break;
        }
    }
    return false;
}

void SettingsDialog::askForSessionRestore(QString device)
{
    if(restoreSession == 1 && sessionRestoreAnswer == -1){
        QMessageBox msgBox;
        msgBox.setText("Session restore");
        msgBox.setInformativeText("Session for "+device+" found. Do you want to restore?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Yes:
            sessionRestoreAnswer = 1;
            break;
        case QMessageBox::No:
        case QMessageBox::Cancel:
            sessionRestoreAnswer = 0;
            break;
        }
    }
}

bool SettingsDialog::askToSaveSession()
{
    if(restoreSession == 0)
        return false;
    if(restoreSession == 2)
        return true;
    if(restoreSession == 1){
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
    }
    return false;
}

void SettingsDialog::setNoSessionFound()
{
    sessionRestoreAnswer = 0;
}

void SettingsDialog::saveSettings()
{
    QSettings settings(settingFile, QSettings::IniFormat);

    settings.setValue("restoreSession", buttonsRestoreSession->getSelectedIndex());
    restoreSession = buttonsRestoreSession->getSelectedIndex();

    settings.setValue("theme", selTheme->getSelectedIndex());
    themeIndex = selTheme->getSelectedIndex();
}

void SettingsDialog::loadSettings()
{
    QFile file(settingFile);
    if(!file.exists()) return;
    QSettings settings(settingFile, QSettings::IniFormat);

    restoreSession = settings.value("restoreSession").toInt();
    buttonsRestoreSession->setChecked(true,restoreSession);

    themeIndex = settings.value("theme").toInt();
    selTheme->setSelected(themeIndex,true);

}

void SettingsDialog::closeDialog(int isCanceled)
{
    if(!isCanceled) saveSettings();
    this->close();
}

void SettingsDialog::restartWarning()
{
    QMessageBox msgBox;
    msgBox.setText("The application needs to be restarted to take effect");
    msgBox.exec();
}
