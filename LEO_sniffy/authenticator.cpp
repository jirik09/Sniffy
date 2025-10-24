#include "authenticator.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QSysInfo>
#include <QDateTime>

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
    authenticationSent = false;
}

void Authenticator::authenticate(const QString &email, const QString &pinHash)
{
    authenticationSent = true;
    if (currentReply)
        return; // already running
    startRequest(email, pinHash, QString(), QString());
    qDebug() << "[Auth] Authentication requested for" << email;
}

void Authenticator::tokenRefresh(const QString &deviceName, const QString &mcuId)
{
    if(currentReply) return; // Request already in progress
    if(authenticationSent == true) return; // Manual authentication already done this session
    const QString email = CustomSettings::getUserEmail();
    if (email.isEmpty() || email == "Unknown user") return;
    qDebug() << "[Auth] Token refresh requested for" << email << "Device:" << deviceName << "MCU_ID:" << mcuId;
    // For refresh, use empty PIN - startRequest will handle it
    startRequest(email, QString(), deviceName, mcuId);
}

void Authenticator::startRequest(const QString &email, const QString &pinHash, const QString &deviceName, const QString &mcuId)
{
    QUrl auth(QStringLiteral("https://sniffy.cz/sniffy_auth2.php"));
    QUrlQuery query;
    query.addQueryItem("email", email);
    if (!pinHash.isEmpty()) query.addQueryItem("pin", pinHash);
    if (!deviceName.isEmpty()) query.addQueryItem("Device_name", deviceName);
    if (!mcuId.isEmpty()) query.addQueryItem("MCU_ID", mcuId);
    auth.setQuery(query);

    QNetworkRequest req(auth);
    const QString userAgent = QString("LEO_sniffy/1.0 (%1; %2; %3; %4)")
            .arg(QSysInfo::machineHostName())
            .arg(QSysInfo::productType())
            .arg(QSysInfo::productVersion())
            .arg(QSysInfo::currentCpuArchitecture());
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    emit requestStarted();
    currentReply = networkManager->get(req);
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

    if (data == "Expired") {
        emit authenticationFailed("expired", QObject::tr("Token expired – login on www.sniffy.cz first"));
        return;
    } else if (data == "wrongPin") {
        emit authenticationFailed("wrong-pin", QObject::tr("Wrong pin or email"));
        return;
    }
    if (data.size() < 20) {
        //emit authenticationFailed("invalid-response-too-short", QObject::tr("Invalid server response"));
        qDebug() << "[Auth] Invalid server response, too short" << data;
        return;
    }
    const QString validityString = QString::fromLatin1(data.left(19));
    const QDateTime validity = QDateTime::fromString(validityString, "yyyy-MM-dd hh:mm:ss");
    const QByteArray token = data.mid(19);
    if (!validity.isValid() || token.isEmpty()) {
        emit authenticationFailed("invalid-response-format", QObject::tr("Invalid response format"));
        return;
    }

    // Update settings on success
    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();

    qDebug() << "[Auth] Token refreshed, valid till:" << validity;
    if(authenticationSent == true){
        qDebug() << "[Auth] Emitting success signal after token refresh";
        emit authenticationSucceeded(validity, token); //this reconnect the device. should not be called on automatic token refresh;
    }
}
