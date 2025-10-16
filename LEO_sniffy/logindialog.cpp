#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QLabel>
#include <QSysInfo>
#include "GUI/clickablelabel.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
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

    // Persistent network manager
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager,&QNetworkAccessManager::finished,this,&LoginDialog::replyFinished);

    // Timeout timer setup (single-shot 15s)
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setInterval(15000);
    connect(&timeoutTimer,&QTimer::timeout,this,[this](){
        if(!requestInFlight || !currentReply) return;
        qInfo() << "[Login] Timeout after 15s";
        CustomSettings::setLastLoginFailure("timeout");
        CustomSettings::saveSettings();
        info->setName("Login timeout");
    info->setColor(Graphics::palette().error);
        currentReply->abort();
        requestInFlight = false;
        buttonsDone->setEnabled(true);
        emit loginFailed("timeout");
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
    CustomSettings::setLastLoginFailure(failureCode);
    CustomSettings::saveSettings();
}

// Helper: start the network request after inputs validated
void LoginDialog::startLoginNetworkRequest(const QString &email, const QString &pinHash){
    CustomSettings::setUserEmail(email);
    CustomSettings::setUserPin(pinHash);
    CustomSettings::saveSettings();

    QUrl auth = QUrl(QStringLiteral("https://sniffy.cz/sniffy_auth.php"));
    QUrlQuery query;
    query.addQueryItem("email", CustomSettings::getUserEmail());
    query.addQueryItem("pin", CustomSettings::getUserPin());
    auth.setQuery(query);

    QNetworkRequest req(auth);
    // Build detailed User-Agent string with system information
    QString userAgent = QString("LEO_sniffy/1.0 (%1; %2; %3; %4)")
        .arg(QSysInfo::machineHostName())
        .arg(QSysInfo::productType())
        .arg(QSysInfo::productVersion())
        .arg(QSysInfo::currentCpuArchitecture());
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    info->setName("Verifying...");
    info->setColor(Graphics::palette().textLabel);
    requestInFlight = true;
    buttonsDone->setEnabled(false);
    qInfo() << "[Login] Sending request for" << email;
    currentReply = networkManager->get(req);
    timeoutTimer.start();
}

// Helper: finalize successful login
void LoginDialog::finalizeSuccess(const QDateTime &validity, const QByteArray &token){
    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    if (logoutLabel) logoutLabel->setVisible(true);

    info->setName("Login valid till: " + CustomSettings::getTokenValidity().toString("dd.MM.yyyy hh:mm"));
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

    if(requestInFlight){
        return; // still processing previous attempt
    }

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
    // Clear stored credentials and token
    CustomSettings::setUserEmail("");
    CustomSettings::setUserPin("");
    CustomSettings::setLoginToken(QByteArray());
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

void LoginDialog::replyFinished(QNetworkReply *pReply)
{
    requestInFlight = false;
    buttonsDone->setEnabled(true);
    timeoutTimer.stop();
    if(currentReply != pReply){
        // Different reply than expected (e.g., aborted). Just ignore if no error.
        qInfo() << "[Login] Received unexpected reply pointer";
    }
    currentReply = nullptr;

    if(pReply->error() != QNetworkReply::NoError){
        reportFailure("Network error: " + pReply->errorString(), "network-error:"+pReply->errorString());
        qInfo() << "[Login] Network error" << pReply->errorString();
        pReply->deleteLater();
        return;
    }

    int httpStatus = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus != 200){
        reportFailure(QString("HTTP %1").arg(httpStatus), QStringLiteral("http-%1").arg(httpStatus));
        qInfo() << "[Login] HTTP error" << httpStatus;
        pReply->deleteLater();
        return;
    }

    QByteArray data = pReply->readAll();
    pReply->deleteLater();

    if(data == "Expired"){
    reportFailure("Token expired – login on www.sniffy.cz","expired", Graphics::palette().warning);
        qInfo() << "[Login] Token expired";
        return;
    }else if(data == "wrongPin"){
        reportFailure("Wrong pin or email","wrong-pin");
        qInfo() << "[Login] Wrong pin";
        return;
    }

    if(data.size() < 20){
        reportFailure("Invalid server response","invalid-response-too-short");
        qInfo() << "[Login] Response too short";
        return;
    }

    QString validityString = QString::fromLatin1(data.left(19));
    QDateTime validity = QDateTime::fromString(validityString, "yyyy-MM-dd hh:mm:ss");
    QByteArray token = data.mid(19);
    if(!validity.isValid() || token.isEmpty()){
        reportFailure("Invalid response format","invalid-response-format");
        qInfo() << "[Login] Invalid response format";
        return;
    }
    finalizeSuccess(validity, token);

}
