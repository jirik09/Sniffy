#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Login");

    setStyleSheet("QWidget{background-color:"+Graphics::COLOR_WINDOW_WIDGET+";}");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(2);
    layout->setContentsMargins(5,5,5,5);

    userEmail= new WidgetTextInput(this,"User email  ");
    userPIN = new WidgetTextInput(this, "PIN  ","",InputTextType::NUMBER);
    userPIN->setAsPassword();
    layout->addWidget(userEmail);
    layout->addWidget(userPIN);
    WidgetSeparator *sep1 = new WidgetSeparator(this,"");
    layout->addWidget(sep1);
    info = new WidgetLabel(this,"First time here? register at www.sniffy.cz"); ////TODO create this as link
    layout->addWidget(info);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer);

    WidgetSeparator *sep2 = new WidgetSeparator(this);
    layout->addWidget(sep2);

    buttonsDone = new WidgetButtons(this,2);
    buttonsDone->setText("  Login  ",0);
    buttonsDone->setText(" Close ",1);
    layout->addWidget(buttonsDone);

    ui->widgetLogin->setLayout(layout);

    userEmail->setText(CustomSettings::getUserEmail());

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
        info->setColor(Graphics::COLOR_ERROR);
        currentReply->abort();
        requestInFlight = false;
        buttonsDone->setEnabled(true);
        emit loginFailed("timeout");
    });

    connect(buttonsDone,&WidgetButtons::clicked,this,&LoginDialog::buttonAction);


}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::open()
{
    //  infoLabel->setValue("");
    //  infoLabel->setColor(Graphics::COLOR_TEXT_ALL);
    this->show();
    this->raise();
    this->activateWindow();
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
        info->setName("Email required");
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed("empty-email");
        CustomSettings::setLastLoginFailure("empty-email");
        CustomSettings::saveSettings();
        return;
    }

    // PIN numeric extraction (defenzivně)
    bool ok=false;
    int pinValue = userPIN->getText().trimmed().toInt(&ok);
    if(!ok){
        info->setName("PIN must be number");
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed("pin-not-numeric");
        CustomSettings::setLastLoginFailure("pin-not-numeric");
        CustomSettings::saveSettings();
        return;
    }

    CustomSettings::setUserEmail(email);
    QString pinHash = QString(QCryptographicHash::hash(QString::number(pinValue + 1297).toUtf8(),QCryptographicHash::Sha1).toHex());
    CustomSettings::setUserPin(pinHash);
    CustomSettings::saveSettings();

    // Prefer HTTPS (případný redirect by prodloužil dobu)
    QUrl auth = QUrl(QStringLiteral("https://sniffy.cz/sniffy_auth.php"));
    QUrlQuery query;
    query.addQueryItem("email", CustomSettings::getUserEmail());
    query.addQueryItem("pin", CustomSettings::getUserPin());
    auth.setQuery(query);

    QNetworkRequest req(auth);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("LEO_sniffy/1.0"));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    info->setName("Verifying...");
    info->setColor(Graphics::COLOR_TEXT_LABEL);
    requestInFlight = true;
    buttonsDone->setEnabled(false); // disable both buttons logically (WidgetButtons manages internally)
    qInfo() << "[Login] Sending request for" << email;
    currentReply = networkManager->get(req);
    timeoutTimer.start();
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
        info->setName("Network error: " + pReply->errorString());
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed("network-error:"+pReply->errorString());
        CustomSettings::setLastLoginFailure("network-error:"+pReply->errorString());
        CustomSettings::saveSettings();
        qInfo() << "[Login] Network error" << pReply->errorString();
        pReply->deleteLater();
        return;
    }

    int httpStatus = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus != 200){
        info->setName(QString("HTTP %1").arg(httpStatus));
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed(QStringLiteral("http-%1").arg(httpStatus));
        CustomSettings::setLastLoginFailure(QStringLiteral("http-%1").arg(httpStatus));
        CustomSettings::saveSettings();
        qInfo() << "[Login] HTTP error" << httpStatus;
        pReply->deleteLater();
        return;
    }

    QByteArray data = pReply->readAll();
    pReply->deleteLater();

    if(data == "Expired"){
        info->setName("Token expired – login on www.sniffy.cz");
        info->setColor(Graphics::COLOR_WARNING);
        emit loginFailed("expired");
        CustomSettings::setLastLoginFailure("expired");
        CustomSettings::saveSettings();
        qInfo() << "[Login] Token expired";
        return;
    }else if(data == "wrongPin"){
        info->setName("Wrong pin or email");
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed("wrong-pin");
        CustomSettings::setLastLoginFailure("wrong-pin");
        CustomSettings::saveSettings();
        qInfo() << "[Login] Wrong pin";
        return;
    }

    if(data.size() < 20){ // minimálně 19 znaků datetime + něco tokenu
        info->setName("Invalid server response");
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed("invalid-response-too-short");
        CustomSettings::setLastLoginFailure("invalid-response-too-short");
        CustomSettings::saveSettings();
        qInfo() << "[Login] Response too short";
        return;
    }

    QString validityString = QString::fromLatin1(data.left(19));
    QDateTime validity = QDateTime::fromString(validityString, "yyyy-MM-dd hh:mm:ss");
    QByteArray token = data.mid(19); // zbytek
    if(!validity.isValid() || token.isEmpty()){
        info->setName("Invalid response format");
        info->setColor(Graphics::COLOR_ERROR);
        emit loginFailed("invalid-response-format");
        CustomSettings::setLastLoginFailure("invalid-response-format");
        CustomSettings::saveSettings();
        qInfo() << "[Login] Invalid response format";
        return;
    }

    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::saveSettings();

    info->setName("Login valid till: " + CustomSettings::getTokenValidity().toString("dd.MM.yyyy hh:mm"));
    info->setColor(Graphics::COLOR_TEXT_ALL);
    emit loginInfoChanged();
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();
    qInfo() << "[Login] Success valid till" << CustomSettings::getTokenValidity();
    // UX: zavřít dialog po úspěchu
    this->close();

}
