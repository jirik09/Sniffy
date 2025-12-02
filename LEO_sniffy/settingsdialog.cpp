#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
                                                  ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");

    setStyleSheet("QWidget{background-color:" + Graphics::palette().windowWidget + ";}");

    QVBoxLayout *buttons = new QVBoxLayout();
    buttons->setSpacing(2);
    buttons->setContentsMargins(5, 5, 5, 5);

    WidgetSeparator *sep1 = new WidgetSeparator(this, "General settings");
    buttons->addWidget(sep1);
    buttonsRestoreSession = new WidgetButtons(this, 3, ButtonTypes::RADIO, "Restore session after startup", 0);
    buttonsRestoreSession->setText("   No   ", 0);
    buttonsRestoreSession->setText("Always ask", 1);
    buttonsRestoreSession->setText("   Yes   ", 2);
    buttons->addWidget(buttonsRestoreSession);

    // Smart session layout and geometry
    buttonsSmartSessionGeometry = new WidgetButtons(this, 2, ButtonTypes::RADIO, "Smart session layout and geometry", 0);
    buttonsSmartSessionGeometry->setText(" Off ", 0);
    buttonsSmartSessionGeometry->setText("  On ", 1);
    buttons->addWidget(buttonsSmartSessionGeometry);

    selTheme = new WidgetSelection(this, "Color scheme");
    QList<QString> *themesList = CustomSettings::getThemesList();
    for (int i = 0; i < themesList->length(); i++)
    {
        selTheme->addOption(themesList->at(i), i);
    }
    buttons->addWidget(selTheme);

    // Session save/load buttons
    WidgetSeparator *sepSession = new WidgetSeparator(this, "");
    buttons->addWidget(sepSession);
    buttonsSession = new WidgetButtons(this, 2, ButtonTypes::NORMAL, "Session");
    buttonsSession->setText("   Save   ", 0);
    buttonsSession->setText("   Load   ", 1);
    buttons->addWidget(buttonsSession);

    // --- Flasher Section ---
    WidgetSeparator *sepFlash = new WidgetSeparator(this, "Firmware Update");
    buttons->addWidget(sepFlash);

    // Flash source selection (Local vs Remote)
    selFlashSource = new WidgetSelection(this, "Source");
    selFlashSource->addOption("Local", 0);    
    selFlashSource->addOption("Remote", 1);
    selFlashSource->setSelected(0);
    selFlashSource->setVisible(false);
    buttons->addWidget(selFlashSource); // Hidden as per new requirements

    buttonsFlash = new WidgetButtons(this, 1, ButtonTypes::NORMAL, "STM32 Firmware");
    buttonsFlash->setText("Install / Flash", 0);
    buttons->addWidget(buttonsFlash);

    flashProgressBar = new SimpleHProgressBar(this);
    flashProgressBar->setRange(0, 100);
    flashProgressBar->setValue(0);
    flashProgressBar->setShowPercent(true);
    flashProgressBar->setVisible(false); // Hide initially
    buttons->addWidget(flashProgressBar);

    flashStatusLabel = new WidgetLabel(this, "", "");
    flashStatusLabel->setVisible(false);
    buttons->addWidget(flashStatusLabel);

    // Initialize Firmware Manager
    m_firmwareManager = new FirmwareManager(this);
    
    connect(buttonsFlash, &WidgetButtons::clicked, this, &SettingsDialog::onFlashButtonClicked);
    
    connect(m_firmwareManager, &FirmwareManager::progressChanged, this, &SettingsDialog::onFirmwareProgress);
    connect(m_firmwareManager, &FirmwareManager::statusMessage, this, &SettingsDialog::onFirmwareStatusMessage);
    connect(m_firmwareManager, &FirmwareManager::operationStarted, this, &SettingsDialog::onFirmwareOperationStarted);
    connect(m_firmwareManager, &FirmwareManager::operationFinished, this, &SettingsDialog::onFirmwareOperationFinished);
    connect(m_firmwareManager, &FirmwareManager::firmwareFlashed, this, &SettingsDialog::onFirmwareFlashed);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    buttons->addItem(verticalSpacer);

    infoLabel = new WidgetLabel(this, "", "");
    buttons->addWidget(infoLabel);

    WidgetSeparator *sep2 = new WidgetSeparator(this);
    buttons->addWidget(sep2);

    buttonsDone = new WidgetButtons(this, 2);
    buttonsDone->setText("  Save  ", 0);
    buttonsDone->setText(" Cancel ", 1);
    buttons->addWidget(buttonsDone);

    ui->widget->setLayout(buttons);

    buttonsRestoreSession->setChecked(true, CustomSettings::getRestoreSession());
    buttonsSmartSessionGeometry->setChecked(true, CustomSettings::getSmartSessionLayoutGeometry() ? 1 : 0);
    selTheme->setSelected(CustomSettings::getThemeIndex());

    connect(buttonsDone, &WidgetButtons::clicked, this, &SettingsDialog::closeDialog);
    connect(selTheme, &WidgetSelection::selectedIndexChanged, this, &SettingsDialog::restartWarning);
    connect(buttonsSession, &WidgetButtons::clicked, this, &SettingsDialog::sessionButtonClicked);
}

SettingsDialog::~SettingsDialog()
{
    // m_firmwareManager is a child of this, so it will be deleted automatically.
    delete ui;
}

void SettingsDialog::open()
{
    infoLabel->setValue("");
    infoLabel->setColor(Graphics::palette().textAll);
    this->show();
    this->raise();
    this->activateWindow();
}

void SettingsDialog::closeDialog(int isCanceled)
{
    if (!isCanceled)
    {
        CustomSettings::setRestoreSession(buttonsRestoreSession->getSelectedIndex());
        CustomSettings::setSmartSessionLayoutGeometry(buttonsSmartSessionGeometry->getSelectedIndex() == 1);
        CustomSettings::setThemeIndex(selTheme->getSelectedIndex());
        CustomSettings::saveSettings();

        if (restartNeededWarning)
        {
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
    infoLabel->setValue("Restart needed to take effect");
    infoLabel->setColor(Graphics::palette().warning);
}

void SettingsDialog::sessionButtonClicked(int index, int optionalEmitParam)
{
    Q_UNUSED(optionalEmitParam);
    if (index == 0)
    {
        // Save
        emit saveSessionRequested();
    }
    else if (index == 1)
    {
        // Load
        emit loadSessionRequested();
    }
}

void SettingsDialog::onFlashButtonClicked(int index, int optionalEmitParam)
{
    Q_UNUSED(index);
    Q_UNUSED(optionalEmitParam);

    if (m_firmwareManager->isFlashInProgress()) {
        flashStatusLabel->setVisible(true);
        flashStatusLabel->setColor(Graphics::palette().warning);
        flashStatusLabel->setValue("Flash already in progress...");
        return;
    }
    
    flashProgressBar->setVisible(true);
    flashStatusLabel->setVisible(true);
    
    m_firmwareManager->startUpdateProcess();
}

void SettingsDialog::onFirmwareProgress(int value, int total)
{
    flashProgressBar->setRange(0, total);
    flashProgressBar->setValue(value);
}

void SettingsDialog::onFirmwareStatusMessage(const QString &msg, const QColor &color)
{
    flashStatusLabel->setValue(msg);
    flashStatusLabel->setColor(color.name());
}

void SettingsDialog::onFirmwareOperationStarted()
{
    buttonsFlash->setEnabled(false);
}

void SettingsDialog::onFirmwareOperationFinished(bool success)
{
    Q_UNUSED(success);
    buttonsFlash->setEnabled(true);
}

void SettingsDialog::onFirmwareFlashed()
{
    emit firmwareFlashed();
}
