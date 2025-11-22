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
    flashStatusLabel->setValue("Connected: " + info + ". Sending ID to server...");

    // Placeholder: "Send" ID to server and "Receive" binary
    // In reality, we just check if file exists on Desktop or use a dummy path for now as requested.
    // User said: "zde proved jen vycteni z disku pro ucel otestovani, z plochy - nazev F303RE_LEO_cube.bin"

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString firmwarePath = desktopPath + "/F303RE_LEO_cube.bin";

    QFile f(firmwarePath);
    if (!f.exists())
    {
        flashStatusLabel->setValue("Error: Firmware file not found on Desktop.");
        flashStatusLabel->setColor(Graphics::palette().error);
        return;
    }

    flashStatusLabel->setValue("Firmware found. Flashing...");

    // Trigger flashing
    QMetaObject::invokeMethod(m_flasher, "flashFirmware", Q_ARG(QString, firmwarePath));
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
