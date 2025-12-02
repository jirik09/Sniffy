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
    selFlashSource->addOption("Local (Desktop)", 0);    
    selFlashSource->addOption("Remote (Web)", 1);
    selFlashSource->setSelected(0);
    buttons->addWidget(selFlashSource);

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

    // Initialize Flasher
    m_flasher = new StLinkFlasher();
    m_flasherThread = new QThread(this);
    m_flasher->moveToThread(m_flasherThread);
    m_flasherThread->start();

    connect(buttonsFlash, &WidgetButtons::clicked, this, &SettingsDialog::onFlashButtonClicked);
    connect(m_flasher, &StLinkFlasher::progressChanged, this, &SettingsDialog::onFlashProgress);
    connect(m_flasher, &StLinkFlasher::logMessage, this, &SettingsDialog::onFlashLog);
    connect(m_flasher, &StLinkFlasher::operationFinished, this, &SettingsDialog::onFlashFinished);
    connect(m_flasher, &StLinkFlasher::deviceConnected, this, &SettingsDialog::onDeviceConnected);
    connect(m_flasher, &StLinkFlasher::operationStarted, this, &SettingsDialog::onOperationStarted);
    connect(m_flasher, &StLinkFlasher::deviceUIDAvailable, this, &SettingsDialog::onDeviceUIDAvailable);
    connect(m_flasher, &StLinkFlasher::deviceUIDError, this, &SettingsDialog::onDeviceUIDError);

    // Authenticator for remote flow
    m_auth = new Authenticator(this);
    connect(m_auth, &Authenticator::requestStarted, this, &SettingsDialog::onAuthStarted);
    connect(m_auth, &Authenticator::requestFinished, this, &SettingsDialog::onAuthFinished);
    connect(m_auth, &Authenticator::authenticationFailed, this, &SettingsDialog::onAuthFailed);
    connect(m_auth, &Authenticator::authenticationSucceeded, this, &SettingsDialog::onAuthSucceeded);

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
    if (m_flasher) {
        // Invoke cleanup in the flasher's thread
        QMetaObject::invokeMethod(m_flasher, "stopAndCleanup");
    } else {
        m_flasherThread->quit();
    }
    
    // Wait for the thread to finish with timeout
    if (!m_flasherThread->wait(5000)) {
        m_flasherThread->terminate();
        m_flasherThread->wait();
    }
    
    delete m_flasher;
    m_flasher = nullptr;
    // m_auth is childed to this via ctor parent, no manual delete needed
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

    if (m_flashInProgress) {
        flashStatusLabel->setVisible(true);
        flashStatusLabel->setColor(Graphics::palette().warning);
        flashStatusLabel->setValue("Flash already in progress...");
        return;
    }

    // Determine source selection
    m_useRemote = (selFlashSource && selFlashSource->getSelectedIndex() == 1);

    flashProgressBar->setVisible(true);
    flashStatusLabel->setVisible(true);
    flashStatusLabel->setColor(Graphics::palette().textAll); // Reset color
    flashStatusLabel->setValue("Connecting to ST-Link...");
    flashProgressBar->setValue(0);

    // Trigger connection in the thread
    QMetaObject::invokeMethod(m_flasher, "connectDevice");
}

void SettingsDialog::onDeviceConnected(const QString &info)
{
    if (m_useRemote)
    {
        flashStatusLabel->setValue("Connected: " + info + ". Reading MCU ID...");
        QMetaObject::invokeMethod(m_flasher, "readDeviceUID");
    }
    else
    {
        // Local: Desktop/sniffy.bin
        QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString firmwarePath = desktopPath + "/sniffy.bin";

        QFile f(firmwarePath);
        if (!f.exists())
        {
            flashStatusLabel->setValue("Error: Firmware file not found on Desktop.");
            flashStatusLabel->setColor(Graphics::palette().error);
            return;
        }

        flashStatusLabel->setValue("Firmware found. Flashing...");
        QMetaObject::invokeMethod(m_flasher, "flashFirmware", Q_ARG(QString, firmwarePath));
    }
}

void SettingsDialog::onFlashProgress(int value, int total)
{
    flashProgressBar->setRange(0, total);
    flashProgressBar->setValue(value);
}

void SettingsDialog::onFlashLog(const QString &msg)
{
    qDebug() << "Flasher:" << msg;
    // Optionally update label with short status
    if (msg.length() < 50)
        flashStatusLabel->setValue(msg);
}

void SettingsDialog::onFlashFinished(bool success, const QString &msg)
{
    flashStatusLabel->setValue(msg);
    if (success)
    {
        flashStatusLabel->setColor(Graphics::palette().running);
        flashProgressBar->setValue(100);
        emit firmwareFlashed();
    }
    else
    {
        flashStatusLabel->setColor(Graphics::palette().error);
    }

    // Re-enable button
    m_flashInProgress = false;
    buttonsFlash->setEnabled(true);

    // Disconnect after operation
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}

void SettingsDialog::onOperationStarted(const QString &operation)
{
    Q_UNUSED(operation);
    m_flashInProgress = true;
    buttonsFlash->setEnabled(false);
}

void SettingsDialog::onDeviceUIDAvailable(const QString &uidHex)
{
    m_lastReadUidHex = uidHex;
    flashStatusLabel->setValue("Contacting server...");
    if (m_auth)
    {
        const QString email = CustomSettings::getUserEmail();
        if (email.isEmpty() || email == "Unknown user")
        {
            flashStatusLabel->setColor(Graphics::palette().warning);
            flashStatusLabel->setValue("Please login first (email required) to request remote firmware.");
            m_flashInProgress = false;
            buttonsFlash->setEnabled(true);
            QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
            return;
        }
        m_auth->tokenRefresh("LEO_sniffy", uidHex);
    }
}

void SettingsDialog::onDeviceUIDError(const QString &message)
{
    flashStatusLabel->setValue("Failed to read MCU ID: " + message);
    flashStatusLabel->setColor(Graphics::palette().error);
    // Re-enable UI and disconnect
    m_flashInProgress = false;
    buttonsFlash->setEnabled(true);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}

void SettingsDialog::onAuthStarted()
{
    // Keep UI busy; already disabled by onOperationStarted
}

void SettingsDialog::onAuthFinished()
{
    // No-op; wait for success/fail signals
}

void SettingsDialog::onAuthFailed(const QString &code, const QString &uiMessage)
{
    Q_UNUSED(code);
    flashStatusLabel->setValue(uiMessage);
    flashStatusLabel->setColor(Graphics::palette().error);
    m_flashInProgress = false;
    buttonsFlash->setEnabled(true);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}

void SettingsDialog::onAuthSucceeded(const QDateTime &validity, const QByteArray &token)
{
    Q_UNUSED(validity);
    Q_UNUSED(token);
    // Placeholder: we need the remote firmware download endpoint and how to authorize
    flashStatusLabel->setColor(Graphics::palette().running);
    // flashStatusLabel->setValue(
    //     "Remote auth OK. TODO: download firmware and flash.\n"
    //     "Required: remote firmware URL/route, auth method (use token?),\n"
    //     "binary selection (device/variant), and checksum/size expectations.");

    // End operation cleanly for now
    m_flashInProgress = false;
    buttonsFlash->setEnabled(true);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}
