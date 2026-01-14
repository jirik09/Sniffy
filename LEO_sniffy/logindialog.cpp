#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QLabel>
#include <QSysInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QDateTime>
#include "GUI/clickablelabel.h"
#include "authenticator.h"

LoginDialog::LoginDialog(Authenticator *authenticator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    auth(authenticator)
{
    ui->setupUi(this);
    this->setWindowTitle("Login");

    setStyleSheet("QWidget{background-color:"+Graphics::palette().windowWidget+";}");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(2);
    layout->setContentsMargins(5,5,5,5);

    userEmail= new WidgetTextInput(this,"User email  ");
    // Use placeholder instead of editable sentinel
    userEmail->setPlaceholder("Unknown user", QColor(160,160,160));
    layout->addWidget(userEmail);
    WidgetSeparator *sep1 = new WidgetSeparator(this,"");
    layout->addWidget(sep1);

    // Static register link label (clickable)
    QLabel *registerLink = new QLabel(this);
    registerLink->setTextFormat(Qt::RichText);
    registerLink->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
    registerLink->setOpenExternalLinks(true);
    registerLink->setAlignment(Qt::AlignHCenter); // center whole block
    registerLink->setStyleSheet(QString(
        "QLabel { color:%1; } "
        "a { color:%2; text-decoration:none; } "
        "a:hover { text-decoration:underline; }")
        .arg(Graphics::palette().textLabel, Graphics::palette().textLabel));
    // Two-line text: second line centered as well, per request (both lines centered)
    registerLink->setText(
        "First time here?<br/>Register at "
        "<a href=\"https://www.sniffy.cz\" target=\"_blank\">www.sniffy.cz</a>");
    layout->addWidget(registerLink);

    // 'info' now strictly for status messages (initially empty)
    info = new WidgetLabel(this,"");
    layout->addWidget(info);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer);

    // Logout label (clickable) placed just before primary action buttons
    logoutLabel = new ClickableLabel(this);
    logoutLabel->setText("Logout");
    logoutLabel->setStyleSheet("color: " + Graphics::palette().textLabel + "; text-decoration: underline; cursor: pointer;");
    logoutLabel->setToolTip("Click to logout");
    logoutLabel->setCursor(Qt::PointingHandCursor);
    // Hide by default unless valid login exists
    logoutLabel->setVisible(CustomSettings::hasValidLogin());
    layout->addWidget(logoutLabel);

    WidgetSeparator *sep2 = new WidgetSeparator(this);
    layout->addWidget(sep2);

    buttonsDone = new WidgetButtons(this,2);
    buttonsDone->setText("Check Login",0);
    buttonsDone->setText(" Close ",1);
    layout->addWidget(buttonsDone);

    ui->widgetLogin->setLayout(layout);

    if(!(CustomSettings::getUserEmail().isEmpty() || CustomSettings::getUserEmail()=="Unknown user")){
        userEmail->setText(CustomSettings::getUserEmail());
    } // else leave empty so placeholder shows

    // Connect to shared authenticator
    connect(auth, &Authenticator::requestStarted, this, [this]() {
        info->setName("Verifying...");
        info->setColor(Graphics::palette().textLabel);
        buttonsDone->setEnabled(false);
    });
    connect(auth, &Authenticator::requestFinished, this, [this]() {
        buttonsDone->setEnabled(true);
    });
    connect(auth, &Authenticator::authenticationFailed, this, [this](const QString &code, const QString &uiMsg){
        qInfo() << "[Login] Authentication failed with code:" << code;
        
        // Check if this is a network/protocol error
        bool isNetworkError = code.startsWith("network-") || code.startsWith("http-") || code.startsWith("invalid-") || code.startsWith("empty-");
        
        // If we are NOT polling (initial check) and it's an auth error (like 'expired', 'error', etc.)
        // then open the browser to start the login flow.
        // Only open browser if this was a user-initiated check (not a background refresh)
        if (auth->isManualAuth() && !isPolling && !isNetworkError) {
            const QString email = userEmail->getText().trimmed();
            if (!email.isEmpty()) {
                qInfo() << "[Login] Auth check failed (" << code << "), opening browser for:" << email;
                info->setName("Opening browser for login...");
                info->setColor(Graphics::palette().textLabel);
                openBrowserForAuth(email);
                return;
            } else {
                qWarning() << "[Login] Auth failed but email is empty!";
            }
        }
        reportFailure(uiMsg, code);
    });
    connect(auth, &Authenticator::authenticationSucceeded, this, [this](const QDateTime &validity, const QByteArray &token, bool forceReconnect){
        finalizeSuccess(validity, token, forceReconnect);
    });

    connect(buttonsDone,&WidgetButtons::clicked,this,&LoginDialog::buttonAction);
    connect(logoutLabel, &ClickableLabel::clicked, this, &LoginDialog::performLogout);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
// Helper: central failure handling to eliminate repetitive code blocks
void LoginDialog::reportFailure(const QString &uiMessage, const QString &failureCode, const QString &color){
    info->setName(uiMessage);
    info->setColor(color);
    emit loginFailed(failureCode);

    // Reset flags on failure
    isPolling = false;

    // Report failure but don't clear token if it was a background renewal
    if (auth && auth->isRenewal()) {
        qDebug() << "[Login] Background renewal failed (" << failureCode << ") - keeping existing token";
        return;
    }

    // Clear token on authentication failure
    CustomSettings::setLoginToken("none");
    qDebug() << "[Login] Clearing stored token due to failure:" << failureCode;
    CustomSettings::setTokenValidity(QDateTime());
    CustomSettings::setLastLoginFailure(failureCode);
    CustomSettings::saveSettings();
}

// Helper: check login status first
void LoginDialog::checkLoginStatus(const QString &email){
    CustomSettings::setUserEmail(email);
    CustomSettings::saveSettings();

    // Reset polling flag for initial check
    isPolling = false;

    qInfo() << "[Login] Checking login status for" << email;
    // Check status - session ID is managed by Authenticator
    auth->checkLogin(email);
}

// Helper: open browser for authentication and start polling
void LoginDialog::openBrowserForAuth(const QString &email){
    qInfo() << "[Login] Opening browser for authentication for" << email;

    // Get the persistent session ID from authenticator
    QString sessionId = auth->getSessionId();

    // Build URL to start authentication directly (no POST, no user_agent)
    QUrl startUrl("https://sniffy.cz/scripts/sniffy_auth_start.php");
    QUrlQuery query;
    query.addQueryItem("email", email);
    query.addQueryItem("session_id", sessionId);
    startUrl.setQuery(query);

    qInfo() << "[Login] Opening browser with URL:" << startUrl.toString();
    if (!QDesktopServices::openUrl(startUrl)) {
        reportFailure("Failed to open browser", "browser-error");
        return;
    }

    qInfo() << "[Login] Browser opened, starting polling for session" << sessionId;
    
    // Set polling flag
    isPolling = true;
    
    // Start polling - session ID is managed by Authenticator
    // Wait for the first interval (5s) to give browser time to load and user to interact
    auth->startPolling(false);
}

// Helper: finalize successful login
void LoginDialog::finalizeSuccess(const QDateTime &validity, const QByteArray &token, bool authenticationSentManual){
    // Reset flags
    isPolling = false;

    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    if (logoutLabel) logoutLabel->setVisible(true);

    info->setName("Login valid till: " + CustomSettings::getTokenValidity().toString("dd.MM.yyyy"));
    info->setColor(Graphics::palette().textAll);
    if(authenticationSentManual){
        emit loginInfoChangedReopen();
    }
    if(this->isVisible()){
        close();
    }
    qInfo() << "[Login] Success valid till" << CustomSettings::getTokenValidity();
}

void LoginDialog::open()
{
    this->show();
    this->raise();
    this->activateWindow();
    if (logoutLabel) logoutLabel->setVisible(CustomSettings::hasValidLogin());
}

void LoginDialog::buttonAction(int isCanceled)
{
    qInfo() << "[Login] Button action triggered. Canceled:" << isCanceled;
    if(isCanceled){
        // Cancel button
        auth->stopPolling();
        isPolling = false;
        this->close();
        return;
    }

    if(!buttonsDone->isEnabled()) {
        qInfo() << "[Login] Buttons disabled, ignoring click";
        return; // Guard
    }

    const QString email = userEmail->getText().trimmed();
    if(email.isEmpty()){
        reportFailure("Email required","empty-email");
        return;
    }

    // First check login status
    checkLoginStatus(email);
}

void LoginDialog::performLogout()
{
    // Stop any active polling
    auth->stopPolling();
    isPolling = false;

    CustomSettings::setUserEmail("");
    CustomSettings::setLoginToken("none");
    CustomSettings::setTokenValidity(QDateTime());
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    userEmail->setText("");
    userEmail->setPlaceholder("Unknown user", QColor(160,160,160));

    emit loginInfoChangedReopen();

    if (logoutLabel) logoutLabel->setVisible(false);
    info->setName("Logged out");
    info->setColor(Graphics::palette().textLabel);
    close();
}

void LoginDialog::reject()
{
    // Stop polling if dialog is closed via Esc or X
    auth->stopPolling();
    isPolling = false;
    QDialog::reject();
}

