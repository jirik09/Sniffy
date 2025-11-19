#include "authenticator.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QSysInfo>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include "customsettings.h"

Authenticator::Authenticator(QObject *parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &Authenticator::onFinished);
    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(15000);
    connect(timeoutTimer, &QTimer::timeout, this, &Authenticator::onTimeout);
    qDebug() << "[Auth] Authenticator initialized";
    authenticationSentManual = false;
}

void Authenticator::authenticate(const QString &email, const QString &pinHash)
{
    authenticationSentManual = true;
    if (currentReply)
        return; // already running
    startRequest(email, pinHash, QString(), QString());
    qDebug() << "[Auth] Authentication requested for" << email;
}

void Authenticator::tokenRefresh(const QString &deviceName, const QString &mcuId)
{
    if(currentReply) return; // Request already in progress
    if(authenticationSentManual == true) return; // Manual authentication already done this session
    const QString email = CustomSettings::getUserEmail();
    if (email.isEmpty() || email == "Unknown user") return;
    
    // Check if token was generated today - if so, skip refresh
    QDate tokenGenDate = CustomSettings::getTokenGeneratedDate();
    if (tokenGenDate.isValid() && tokenGenDate == QDate::currentDate()) {
        qDebug() << "[Auth] Token refresh skipped - token was generated today";
        return;
    }
    
    qDebug() << "[Auth] Token refresh requested for" << email << "Device:" << deviceName << "MCU_ID:" << mcuId;
    // For refresh, use empty PIN - startRequest will handle it
    startRequest(email, QString(), deviceName, mcuId);
}

void Authenticator::startRequest(const QString &email, const QString &pinHash, const QString &deviceName, const QString &mcuId)
{
    QUrl auth(QStringLiteral("https://sniffy.cz/sniffy_auth_new.php"));
    
    QUrlQuery query;
    query.addQueryItem("email", email);
    if (!pinHash.isEmpty()) query.addQueryItem("pin", pinHash);
    if (!deviceName.isEmpty()) query.addQueryItem("Device_name", deviceName);
    if (!mcuId.isEmpty()) query.addQueryItem("MCU_ID", mcuId);
    
    QNetworkRequest req(auth);
    const QString userAgent = QString("LEO_sniffy/1.0 (%1; %2; %3; %4)")
            .arg(QSysInfo::machineHostName())
            .arg(QSysInfo::productType())
            .arg(QSysInfo::productVersion())
            .arg(QSysInfo::currentCpuArchitecture());
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    if(authenticationSentManual == true){
        emit requestStarted();
    }

    QByteArray postData = query.toString(QUrl::FullyEncoded).toUtf8();
    currentReply = networkManager->post(req, postData);
    if (timeoutTimer) timeoutTimer->start();
}

void Authenticator::onTimeout()
{
    if (currentReply) currentReply->abort();
}

void Authenticator::onFinished(QNetworkReply *reply)
{
    if (timeoutTimer) timeoutTimer->stop();
    if (reply != currentReply) { reply->deleteLater(); return; }
    currentReply = nullptr;
    emit requestFinished();

    if (reply->error() != QNetworkReply::NoError) {
        const QString e = reply->errorString();
        reply->deleteLater();
        emit authenticationFailed(QStringLiteral("network-error:%1").arg(e), QStringLiteral("Network error: %1").arg(e));
        return;
    }
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatus != 200) {
        reply->deleteLater();
        emit authenticationFailed(QStringLiteral("http-%1").arg(httpStatus), QStringLiteral("HTTP %1").arg(httpStatus));
        return;
    }

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    // Parse JSON response
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit authenticationFailed("invalid-json", QObject::tr("Invalid JSON response: %1").arg(parseError.errorString()));
        qDebug() << "[Auth] JSON parse error:" << parseError.errorString() << "Data:" << data;
        return;
    }

    if (!jsonDoc.isObject()) {
        emit authenticationFailed("invalid-response-format", QObject::tr("Invalid response format"));
        qDebug() << "[Auth] Response is not a JSON object";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    
    // Check for error responses
    if (jsonObj.contains("error")) {
        QString errorType = jsonObj["error"].toString();
        if (errorType == "Expired") {
            emit authenticationFailed("expired", QObject::tr("Token expired – login on www.sniffy.cz first"));
            return;
        } else if (errorType == "wrongPin") {
            emit authenticationFailed("wrong-pin", QObject::tr("Wrong pin or email"));
            return;
        } else {
            emit authenticationFailed(errorType, QObject::tr("Authentication error: %1").arg(errorType));
            return;
        }
    }

    // Extract valid_till and token from JSON
    if (!jsonObj.contains("valid_till") || !jsonObj.contains("token")) {
        emit authenticationFailed("missing-fields", QObject::tr("Response missing required fields"));
        qDebug() << "[Auth] Missing valid_till or token in response";
        return;
    }

    const QString validityString = jsonObj["valid_till"].toString();
    const QString tokenString = jsonObj["token"].toString();
    
    if (validityString.isEmpty() || tokenString.isEmpty()) {
        emit authenticationFailed("empty-fields", QObject::tr("Empty validity or token"));
        return;
    }

    // Parse the validity date - try ISO format first, then fallback to original format
    QDateTime validity = QDateTime::fromString(validityString, Qt::ISODate);
    if (!validity.isValid()) {
        validity = QDateTime::fromString(validityString, "yyyy-MM-dd hh:mm:ss");
    }
    if (!validity.isValid()) {
        validity = QDateTime::fromString(validityString, "yyyy-MM-dd");
    }
    
    const QByteArray token = tokenString.toLatin1();
    
    if (!validity.isValid() || token.isEmpty()) {
        emit authenticationFailed("invalid-response-format", QObject::tr("Invalid response format"));
        qDebug() << "[Auth] Invalid validity date or empty token";
        return;
    }

    QDateTime previousValidity = CustomSettings::getTokenValidity();
    bool forceReconnect = authenticationSentManual;
    if (authenticationSentManual || QDateTime::currentDateTime() > previousValidity) {
        forceReconnect = true;
        qDebug() << "[Auth] New Token generated - forcing device reconnection";
    }
    
    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::setTokenGeneratedDate(QDate::currentDate());
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    qDebug() << "[Auth] Token refreshed, valid till:" << validity;
    
    emit authenticationSucceeded(validity, token, forceReconnect); //if forceReconnect is true this reconnect the device. 
    if(!forceReconnect){
        // request small popup only on automatic prolongation
        const QString tip = QObject::tr("Session extended. Valid till: %1")
                                .arg(validity.date().toString("dd.MM.yyyy"));
        emit popupMessageRequested(tip);
    }
}
