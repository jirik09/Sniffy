#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QLabel>
#include <QSysInfo>
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
    userPIN = new WidgetTextInput(this, "PIN  ", "", InputTextType::NUMBER);
    userPIN->setAsPassword();
    layout->addWidget(userEmail);
    layout->addWidget(userPIN);
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
    buttonsDone->setText("  Login  ",0);
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
        reportFailure(uiMsg, code);
    });
    connect(auth, &Authenticator::authenticationSucceeded, this, [this](const QDateTime &validity, const QByteArray &token){
        finalizeSuccess(validity, token);
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
    
    // Clear token on authentication failure
    CustomSettings::setLoginToken("none");
    CustomSettings::setTokenValidity(QDateTime());
    CustomSettings::setLastLoginFailure(failureCode);
    CustomSettings::saveSettings();
}

// Helper: start the network request after inputs validated
void LoginDialog::startLoginNetworkRequest(const QString &email, const QString &pinHash){
    CustomSettings::setUserEmail(email);
    CustomSettings::saveSettings();

    qInfo() << "[Login] Sending request for" << email;
    // Delegate to shared authenticator (no Device_name at explicit login)
    auth->authenticate(email, pinHash);
}

// Helper: finalize successful login
void LoginDialog::finalizeSuccess(const QDateTime &validity, const QByteArray &token){
    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    if (logoutLabel) logoutLabel->setVisible(true);

    info->setName("Login valid till: " + CustomSettings::getTokenValidity().toString("dd.MM.yyyy"));
    info->setColor(Graphics::palette().textAll);
    emit loginInfoChanged();
    qInfo() << "[Login] Success valid till" << CustomSettings::getTokenValidity();
    close();
}

void LoginDialog::open()
{
    //  infoLabel->setValue("");
    //  (legacy) infoLabel color line removed after palette migration
    this->show();
    this->raise();
    this->activateWindow();
    if (logoutLabel) logoutLabel->setVisible(CustomSettings::hasValidLogin());
}

void LoginDialog::buttonAction(int isCanceled)
{
    if(isCanceled){
        this->close();
        return;
    }

    // Re-entrancy guard via disabled button; no extra flag needed
    if(!buttonsDone->isEnabled()) return;

    const QString email = userEmail->getText().trimmed();
    if(email.isEmpty()){
        reportFailure("Email required","empty-email");
        return;
    }

    // PIN numeric extraction (defensive)
    bool ok=false;
    int pinValue = userPIN->getText().trimmed().toInt(&ok);
    if(!ok){
        reportFailure("PIN must be number","pin-not-numeric");
        return;
    }

    QString pinHash = QString(QCryptographicHash::hash(QString::number(pinValue + 1297).toUtf8(),QCryptographicHash::Sha1).toHex());
    startLoginNetworkRequest(email, pinHash);
}

void LoginDialog::performLogout()
{
    CustomSettings::setUserEmail("");
    CustomSettings::setLoginToken("none");
    CustomSettings::setTokenValidity(QDateTime());
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    userEmail->setText("");
    userEmail->setPlaceholder("Unknown user", QColor(160,160,160));
    userPIN->setText("");

    // Notify the rest of the app that login state changed
    emit loginInfoChanged();

    // Update UI accordingly and close dialog
    if (logoutLabel) logoutLabel->setVisible(false);
    info->setName("Logged out");
    info->setColor(Graphics::palette().textLabel);
    close();
}

// legacy slot not used anymore
