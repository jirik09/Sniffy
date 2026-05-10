#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardPaths>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>

namespace
{

    constexpr int kSettingsMenuWidth = 114;
    constexpr int kMinimumSettingsDialogHeight = 420;

    QString applicationVersionText()
    {
#ifdef APP_VERSION
        return QString::fromLatin1(APP_VERSION);
#else
        return QStringLiteral("x.0.0");
#endif
    }

    QString readBundledDocument(const QString &resourcePath)
    {
        QFile file(resourcePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return QString("Unable to load bundled document:\n%1").arg(resourcePath);
        }

        return QString::fromUtf8(file.readAll());
    }

    QTextBrowser *createBundledDocumentView(QWidget *parent, const QString &resourcePath, bool useMarkdown)
    {
        auto *view = new QTextBrowser(parent);
        view->setReadOnly(true);
        view->setOpenExternalLinks(true);
        view->setStyleSheet(QString(
                                "QTextBrowser {"
                                "background-color: %1;"
                                "color: %2;"
                                "border: 1px solid %3;"
                                "}")
                                .arg(Graphics::palette().backgroundFocusIn,
                                     Graphics::palette().textAll,
                                     Graphics::palette().textLabel));

        const QString content = readBundledDocument(resourcePath);
        if (useMarkdown)
        {
            view->document()->setBaseUrl(QUrl("qrc:/licenses/"));
            view->setMarkdown(content);
        }
        else
        {
            view->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
            view->setPlainText(content);
        }

        return view;
    }

    QVBoxLayout *createSettingsPageLayout(QWidget *parent)
    {
        auto *layout = new QVBoxLayout(parent);
        layout->setSpacing(2);
        layout->setContentsMargins(0, 0, 0, 0);
        return layout;
    }

    QPushButton *createSettingsMenuButton(QWidget *parent, const QString &label)
    {
        auto *button = new QPushButton(label, parent);
        button->setCheckable(true);
        button->setCursor(Qt::PointingHandCursor);
        button->setMinimumHeight(34);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        return button;
    }

} // namespace

SettingsDialog::SettingsDialog(Authenticator *auth, QWidget *parent) : QDialog(parent),
                                                                       ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");

    setStyleSheet("QWidget{background-color:" + Graphics::palette().windowWidget + ";}");

    const int expandedWidth = width() + kSettingsMenuWidth + 20;
    const int expandedHeight = qMax(height(), kMinimumSettingsDialogHeight);
    resize(expandedWidth, expandedHeight);
    setMinimumSize(expandedWidth, expandedHeight);

    auto *dialogLayout = new QVBoxLayout();
    dialogLayout->setSpacing(2);
    dialogLayout->setContentsMargins(5, 5, 5, 5);

    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(10);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    auto *navigationPanel = new QWidget(ui->widget);
    navigationPanel->setObjectName("settingsNavigationPanel");
    navigationPanel->setFixedWidth(kSettingsMenuWidth);
    navigationPanel->setStyleSheet(QString(
                                       "QWidget#settingsNavigationPanel {"
                                       "background-color: %1;"
                                       "border: 1px solid %2;"
                                       "}"
                                       "QPushButton {"
                                       "background-color: transparent;"
                                       "color: %3;"
                                       "border: 1px solid transparent;"
                                       "border-left: 3px solid transparent;"
                                       "padding: 7px 10px;"
                                       "text-align: left;"
                                       "font-weight: 600;"
                                       "}"
                                       "QPushButton:hover {"
                                       "background-color: %4;"
                                       "border-color: %2;"
                                       "}"
                                       "QPushButton:checked {"
                                       "background-color: %4;"
                                       "color: %5;"
                                       "border-color: %2;"
                                       "border-left: 3px solid %6;"
                                       "}")
                                       .arg(Graphics::palette().windowWidget,
                                            Graphics::palette().textLabel,
                                            Graphics::palette().textLabel,
                                            Graphics::palette().backgroundFocusIn,
                                            Graphics::palette().textAll,
                                            Graphics::palette().warning));

    auto *navigationLayout = new QVBoxLayout(navigationPanel);
    navigationLayout->setSpacing(2);
    navigationLayout->setContentsMargins(0, 0, 0, 0);

    auto *pages = new QStackedWidget(ui->widget);

    auto *navigationGroup = new QButtonGroup(this);
    navigationGroup->setExclusive(true);

    auto *generalButton = createSettingsMenuButton(navigationPanel, "General");
    auto *appearanceButton = createSettingsMenuButton(navigationPanel, "Appearance");
    auto *fwUpdateButton = createSettingsMenuButton(navigationPanel, "Firmware");
    auto *aboutButton = createSettingsMenuButton(navigationPanel, "About");

    navigationGroup->addButton(generalButton, 0);
    navigationGroup->addButton(appearanceButton, 1);
    navigationGroup->addButton(fwUpdateButton, 2);
    navigationGroup->addButton(aboutButton, 3);

    navigationLayout->addWidget(generalButton);
    navigationLayout->addWidget(appearanceButton);
    navigationLayout->addWidget(fwUpdateButton);
    navigationLayout->addWidget(aboutButton);
    navigationLayout->addStretch();

    auto *generalPage = new QWidget(pages);
    auto *generalLayout = createSettingsPageLayout(generalPage);

    WidgetSeparator *sepSession = new WidgetSeparator(generalPage, "Session");
    generalLayout->addWidget(sepSession);

    buttonsRestoreSession = new WidgetButtons(generalPage, 3, ButtonTypes::RADIO, "Restore session after startup", 0);
    buttonsRestoreSession->setText("   No   ", 0);
    buttonsRestoreSession->setText("Always ask", 1);
    buttonsRestoreSession->setText("   Yes   ", 2);
    generalLayout->addWidget(buttonsRestoreSession);

    // Smart session layout and geometry
    buttonsSmartSessionGeometry = new WidgetButtons(generalPage, 2, ButtonTypes::RADIO, "Smart session layout and geometry", 0);
    buttonsSmartSessionGeometry->setText(" Off ", 0);
    buttonsSmartSessionGeometry->setText("  On ", 1);
    generalLayout->addWidget(buttonsSmartSessionGeometry);

    buttonsSession = new WidgetButtons(generalPage, 2, ButtonTypes::NORMAL, "Session");
    buttonsSession->setText("   Save   ", 0);
    buttonsSession->setText("   Load   ", 1);
    generalLayout->addWidget(buttonsSession);
    generalLayout->addStretch();

    pages->addWidget(generalPage);

    auto *appearancePage = new QWidget(pages);
    auto *appearanceLayout = createSettingsPageLayout(appearancePage);

    WidgetSeparator *sepTheme = new WidgetSeparator(appearancePage, "App theme");
    appearanceLayout->addWidget(sepTheme);

    selTheme = new WidgetSelection(appearancePage, "Theme");
    QList<QString> *themesList = CustomSettings::getThemesList();
    for (int i = 0; i < themesList->length(); i++)
    {
        selTheme->addOption(themesList->at(i), i);
    }
    appearanceLayout->addWidget(selTheme);
    appearanceLayout->addStretch();

    pages->addWidget(appearancePage);

    auto *firmwarePage = new QWidget(pages);
    auto *firmwareLayout = createSettingsPageLayout(firmwarePage);

    // --- Flasher Section ---
    WidgetSeparator *sepFlash = new WidgetSeparator(firmwarePage, "Firmware Update");
    firmwareLayout->addWidget(sepFlash);

    // Flash source selection (Local vs Remote)
    selFlashSource = new WidgetSelection(firmwarePage, "Source");
    selFlashSource->addOption("Local", 0);
    selFlashSource->addOption("Remote", 1);
    selFlashSource->setSelected(0);
    selFlashSource->setVisible(false);
    firmwareLayout->addWidget(selFlashSource); // Hidden as per new requirements

    buttonsFlash = new WidgetButtons(firmwarePage, 1, ButtonTypes::NORMAL, "STM32 Firmware");
    buttonsFlash->setText("Install / Flash", 0);
    firmwareLayout->addWidget(buttonsFlash);

    flashProgressBar = new SimpleHProgressBar(firmwarePage);
    flashProgressBar->setRange(0, 100);
    flashProgressBar->setValue(0);
    flashProgressBar->setShowPercent(true);
    flashProgressBar->setVisible(false); // Hide initially
    firmwareLayout->addWidget(flashProgressBar);

    flashLogWindow = new QPlainTextEdit(firmwarePage);
    flashLogWindow->setReadOnly(true);
    flashLogWindow->setMinimumHeight(150);
    flashLogWindow->setVisible(false);
    // set background of the log window
    auto getFlashLogStyleSheet = []() -> QString
    {
        return QString(
                   "QPlainTextEdit {\n"
                   "    background-color: %1;\n"
                   "    color: %2;\n"
                   "}")
            .arg(Graphics::palette().backgroundFocusIn, Graphics::palette().textAll);
    };
    flashLogWindow->setStyleSheet(getFlashLogStyleSheet());
    firmwareLayout->addWidget(flashLogWindow);

    buttonsErase = new WidgetButtons(firmwarePage, 1, ButtonTypes::NORMAL, "Erase firmware");
    buttonsErase->setText("Mass Erase", 0);
    firmwareLayout->addWidget(buttonsErase);
    connect(buttonsErase, &WidgetButtons::clicked, this, &SettingsDialog::onEraseButtonClicked);

    firmwareLayout->addStretch();

    pages->addWidget(firmwarePage);

    auto *aboutPage = new QWidget(pages);
    auto *aboutLayout = createSettingsPageLayout(aboutPage);

    WidgetSeparator *sepAbout = new WidgetSeparator(aboutPage, "About");
    aboutLayout->addWidget(sepAbout);

    auto *labelAppVersion = new WidgetLabel(aboutPage, "Application version", applicationVersionText());
    labelAppVersion->setValueAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    aboutLayout->addWidget(labelAppVersion);

    WidgetSeparator *sepLegal = new WidgetSeparator(aboutPage, "Legal");
    aboutLayout->addWidget(sepLegal);

    buttonsLicenses = new WidgetButtons(aboutPage, 1, ButtonTypes::NORMAL, "Open-source notices");
    buttonsLicenses->setText("Third-party licenses", 0);
    aboutLayout->addWidget(buttonsLicenses);
    aboutLayout->addStretch();

    pages->addWidget(aboutPage);

    // Initialize Firmware Manager
    m_firmwareManager = new FirmwareManager(auth, this);

    connect(buttonsFlash, &WidgetButtons::clicked, this, &SettingsDialog::onFlashButtonClicked);

    connect(m_firmwareManager, &FirmwareManager::progressChanged, this, &SettingsDialog::onFirmwareProgress);
    connect(m_firmwareManager, &FirmwareManager::statusMessage, this, &SettingsDialog::onFirmwareStatusMessage);
    connect(m_firmwareManager, &FirmwareManager::logMessage, this, &SettingsDialog::onFirmwareLogMessage);
    connect(m_firmwareManager, &FirmwareManager::operationStarted, this, &SettingsDialog::onFirmwareOperationStarted);
    connect(m_firmwareManager, &FirmwareManager::operationFinished, this, &SettingsDialog::onFirmwareOperationFinished);
    connect(m_firmwareManager, &FirmwareManager::firmwareFlashed, this, &SettingsDialog::onFirmwareFlashed);
    connect(buttonsLicenses, &WidgetButtons::clicked, this, &SettingsDialog::showThirdPartyLicenses);

    connect(navigationGroup, QOverload<int>::of(&QButtonGroup::idClicked), pages, &QStackedWidget::setCurrentIndex);

    generalButton->setChecked(true);
    pages->setCurrentIndex(0);

    contentLayout->addWidget(navigationPanel);
    contentLayout->addWidget(pages, 1);

    dialogLayout->addLayout(contentLayout, 1);

    infoLabel = new WidgetLabel(ui->widget, "", "");
    dialogLayout->addWidget(infoLabel);

    WidgetSeparator *sep2 = new WidgetSeparator(ui->widget);
    dialogLayout->addWidget(sep2);

    buttonsDone = new WidgetButtons(ui->widget, 2);
    buttonsDone->setText("  Save  ", 0);
    buttonsDone->setText(" Cancel ", 1);
    dialogLayout->addWidget(buttonsDone);

    ui->widget->setLayout(dialogLayout);

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

    // Clear flash status if it was a login error and we are now logged in
    if (m_lastStatusType == FirmwareManager::MsgLoginRequired && CustomSettings::hasValidLogin())
    {
        flashLogWindow->clear();
        flashLogWindow->setVisible(false);
        m_lastStatusType = FirmwareManager::MsgInfo;
    }

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

    if (m_firmwareManager->isFlashInProgress())
    {
        flashLogWindow->setVisible(true);
        flashLogWindow->appendHtml(QString("<font color=\"%1\">%2</font>").arg(Graphics::palette().warning, "Flash already in progress..."));
        return;
    }

    flashProgressBar->setVisible(true);
    flashLogWindow->setVisible(true);
    flashLogWindow->clear();

    m_firmwareManager->startUpdateProcess();
}

void SettingsDialog::onEraseButtonClicked(int index, int optionalEmitParam)
{
    Q_UNUSED(index);
    Q_UNUSED(optionalEmitParam);

    if (m_firmwareManager->isFlashInProgress())
    {
        flashLogWindow->setVisible(true);
        flashLogWindow->appendHtml(QString("<font color=\"%1\">%2</font>").arg(Graphics::palette().warning, "Operation already in progress..."));
        return;
    }
    flashProgressBar->setVisible(true);
    flashLogWindow->setVisible(true);
    flashLogWindow->clear();
    m_firmwareManager->startMassErase();
}

void SettingsDialog::showThirdPartyLicenses()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Third-party notices");
    dialog.resize(900, 650);
    dialog.setStyleSheet("QWidget{background-color:" + Graphics::palette().windowWidget + ";}");

    auto *layout = new QVBoxLayout(&dialog);
    auto *tabs = new QTabWidget(&dialog);

    tabs->addTab(createBundledDocumentView(tabs, ":/licenses/THIRD-PARTY-NOTICES.md", true), "Overview");
    tabs->addTab(createBundledDocumentView(tabs, ":/licenses/third-party-licenses/libusb-LGPL-2.1.txt", false), "libusb LGPL");
    tabs->addTab(createBundledDocumentView(tabs, ":/licenses/third-party-licenses/stlink-BSD-3-Clause.txt", false), "stlink BSD");
    tabs->addTab(createBundledDocumentView(tabs, ":/licenses/third-party-licenses/tinyexpr-zlib.txt", false), "tinyexpr zlib");

    layout->addWidget(tabs);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    dialog.exec();
}

void SettingsDialog::onFirmwareProgress(int value, int total)
{
    flashProgressBar->setRange(0, total);
    flashProgressBar->setValue(value);
}

void SettingsDialog::onFirmwareStatusMessage(const QString &msg, const QColor &color, int msgType)
{
    m_lastStatusType = msgType;
    flashLogWindow->setVisible(true);
    flashLogWindow->appendHtml(QString("<font color=\"%1\">%2</font>").arg(color.name(), msg));
    // Auto-scroll to bottom
    QScrollBar *sb = flashLogWindow->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void SettingsDialog::onFirmwareLogMessage(const QString &message)
{
    flashLogWindow->setVisible(true);
    flashLogWindow->appendPlainText(message);
    // Auto-scroll to bottom
    QScrollBar *sb = flashLogWindow->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void SettingsDialog::onUserLoginChanged()
{
    // If the last error was about login, and now we have a valid login, clear the error
    if (m_lastStatusType == FirmwareManager::MsgLoginRequired && CustomSettings::hasValidLogin())
    {
        flashLogWindow->clear();
        flashLogWindow->setVisible(false);
        m_lastStatusType = FirmwareManager::MsgInfo; // Reset status
    }
}

void SettingsDialog::onFirmwareOperationStarted()
{
    buttonsFlash->setEnabled(false);
    buttonsErase->setEnabled(false);
}

void SettingsDialog::onFirmwareOperationFinished(bool success)
{
    Q_UNUSED(success);
    buttonsFlash->setEnabled(true);
    buttonsErase->setEnabled(true);
}

void SettingsDialog::onFirmwareFlashed()
{
    emit firmwareFlashed();
}
