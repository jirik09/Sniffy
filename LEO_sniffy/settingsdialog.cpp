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
    QList<QString> themesList = CustomSettings::getThemesList();
    for (int i =0; i<themesList.length(); i++) {
        selTheme->addOption(themesList.at(i),i);
    }
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


    buttonsRestoreSession->setChecked(true,CustomSettings::getRestoreSession());
    selTheme->setSelected(CustomSettings::getThemeIndex());

    connect(buttonsDone,&WidgetButtons::clicked,this,&SettingsDialog::closeDialog);
    connect(selTheme,&WidgetSelection::selectedIndexChanged,this,&SettingsDialog::restartWarning);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::open()
{
    this->show();
    this->raise();
    this->activateWindow();
}

void SettingsDialog::closeDialog(int isCanceled)
{
    if(!isCanceled){
        CustomSettings::setRestoreSession(buttonsRestoreSession->getSelectedIndex());
        CustomSettings::setThemeIndex(selTheme->getSelectedIndex());
        CustomSettings::saveSettings();

        if(restartNeededWarning){
            QMessageBox msgBox;
            msgBox.setText("The application needs to be restarted to take effect");
            msgBox.exec();
        }
        restartNeededWarning = 0;
    }
    this->close();
}

void SettingsDialog::restartWarning()
{
    restartNeededWarning = true;
}
