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
    timeoutTimer->setInterval(15000); // 15 seconds timeout for network requests
    connect(timeoutTimer, &QTimer::timeout, this, &Authenticator::onTimeout);
    
    pollTimer = new QTimer(this);
    pollTimer->setInterval(3000); // Poll after 3seconds used for login status checking
    connect(pollTimer, &QTimer::timeout, this, &Authenticator::checkAuthStatus);
    
    qDebug() << "[Auth] Authenticator initialized";
    authenticationSentManual = false;
    
    // Generate persistent session ID for this application run
    currentSessionId = QString::number(QDateTime::currentMSecsSinceEpoch());
    qDebug() << "[Auth] Session ID generated:" << currentSessionId;
}

void Authenticator::checkLogin(const QString &email)
{
    authenticationSentManual = true;
    if (currentReply)
        return; // already running
    
    // Ensure polling is stopped for manual check
    if (pollTimer->isActive()) pollTimer->stop();
    
    qDebug() << "[Auth] Checking login status for" << email << "Session:" << currentSessionId;
    emit requestStarted();
    startRequest(email, QString(), QString(), false);
}

void Authenticator::setConnectedDevice(const QString &name, const QString &id)
{
    qDebug() << "[Auth] Device info updated:" << name << id;
    currentDeviceName = name;
    currentMcuId = id;
}

void Authenticator::startPolling(bool immediate)
{
    qDebug() << "[Auth] Starting polling for session:" << currentSessionId << "Immediate:" << immediate;
    emit requestStarted();
    pollTimer->start();
    if (immediate) {
        checkAuthStatus(); // Check immediately
    }
}

void Authenticator::stopPolling()
{
    pollTimer->stop();
    // Do NOT clear currentSessionId - it persists for the app lifetime
    if (currentReply) {
        currentReply->abort();
        currentReply = nullptr;
    }
    emit requestFinished();
    qDebug() << "[Auth] Polling stopped";
}

void Authenticator::tokenRefresh(const QString &deviceName, const QString &mcuId, bool isDemoMode)
{
    if(currentReply) return; // Request already in progress
    const QString email = CustomSettings::getUserEmail();
    if (email.isEmpty() || email == "Unknown user") return;
    
    qDebug() << "[Auth] Token refresh requested for" << email << "Device:" << deviceName << "MCU_ID:" << mcuId << "Demo:" << isDemoMode;
    // Store demo mode state
    isDemoModeRequest = isDemoMode;
    // For refresh, use the renewal flow with device info
    startRequest(email, deviceName, mcuId, true);
}

void Authenticator::startRequest(const QString &email, const QString &deviceName, const QString &mcuId, bool isRenewal)
{
    // Store the renewal flag for this request
    isRenewalRequest = isRenewal;
    if (!isRenewal) {
        // Reset demo mode request flag for manual/polling requests
        isDemoModeRequest = false;
    }
    qCritical() << "[Auth] Starting" << (isRenewal ? "renewal" : "auth check") << "request for" << email 
                << "Device:" << deviceName << "MCU_ID:" << mcuId 
                << "Session:" << currentSessionId 
                << "Demo mode request:" << isDemoModeRequest;
    
    // Use auth_check for renewal or polling, auth_start is only used from browser
    QUrl authUrl(QStringLiteral("https://sniffy.cz/scripts/sniffy_auth_check.php"));
    
    QUrlQuery query;
    query.addQueryItem("email", email);
    if (!deviceName.isEmpty()) query.addQueryItem("Device_name", deviceName);
    if (!mcuId.isEmpty()) query.addQueryItem("MCU_ID", mcuId);
    if (!currentSessionId.isEmpty()) query.addQueryItem("session_id", currentSessionId);
    
    QNetworkRequest req(authUrl);
    const QString userAgent = QString("LEO_sniffy/1.0 (%1; %2; %3; %4)")
            .arg(QSysInfo::machineHostName())
            .arg(QSysInfo::productType())
            .arg(QSysInfo::productVersion())
            .arg(QSysInfo::currentCpuArchitecture());
    req.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    if(authenticationSentManual == true && currentSessionId.isEmpty()){
        // Only emit requestStarted if not polling (polling already emitted it)
        emit requestStarted();
    }

    QByteArray postData = query.toString(QUrl::FullyEncoded).toUtf8();
    qDebug() << "[Auth] Sending POST data to" << authUrl.toString() << ":" << postData;
    currentReply = networkManager->post(req, postData);
    if (timeoutTimer) timeoutTimer->start();
}

void Authenticator::onTimeout()
{
    if (currentReply) currentReply->abort();
}

void Authenticator::checkAuthStatus()
{
    // If polling is not active, stop (safety check)
    if (!pollTimer->isActive()) {
        return;
    }
    
    if (currentReply) return; // Previous request still in progress
    
    const QString email = CustomSettings::getUserEmail();
    qDebug() << "[Auth] Polling auth status for session:" << currentSessionId;
    startRequest(email, currentDeviceName, currentMcuId, false);
}

void Authenticator::onFinished(QNetworkReply *reply)
{
    if (timeoutTimer) timeoutTimer->stop();
    if (reply != currentReply) { reply->deleteLater(); return; }
    currentReply = nullptr;
    
    // Don't emit requestFinished during polling - keep the UI in "waiting" state
    if (!pollTimer->isActive()) {
        emit requestFinished();
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString e = reply->errorString();
        reply->deleteLater();
        // During polling, network errors don't stop polling - just log and continue
        if (pollTimer->isActive()) {
            qDebug() << "[Auth] Network error during polling:" << e << "- continuing";
            return;
        }
        emit authenticationFailed(QStringLiteral("network-error:%1").arg(e), QStringLiteral("Network error: %1").arg(e));
        return;
    }
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatus != 200) {
        reply->deleteLater();
        // During polling, HTTP errors don't stop polling - just log and continue
        if (pollTimer->isActive()) {
            qDebug() << "[Auth] HTTP error during polling:" << httpStatus << "- continuing";
            return;
        }
        emit authenticationFailed(QStringLiteral("http-%1").arg(httpStatus), QStringLiteral("HTTP %1").arg(httpStatus));
        return;
    }

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    // Debug: Print the raw response
    qDebug() << "[Auth] Raw response from sniffy_auth_check.php:" << data;
    qDebug() << "[Auth] Response size:" << data.size() << "bytes";

    // Parse JSON response
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        // During polling, parsing errors don't stop polling
        if (pollTimer->isActive()) {
            qDebug() << "[Auth] JSON parse error during polling - continuing";
            return;
        }
        emit authenticationFailed("invalid-json", QObject::tr("Invalid JSON response: %1").arg(parseError.errorString()));
        qDebug() << "[Auth] JSON parse error:" << parseError.errorString() << "Data:" << data;
        return;
    }

    if (!jsonDoc.isObject()) {
        if (pollTimer->isActive()) {
            qDebug() << "[Auth] Invalid response format during polling - continuing";
            return;
        }
        emit authenticationFailed("invalid-response-format", QObject::tr("Invalid response format"));
        qDebug() << "[Auth] Response is not a JSON object";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    
    // Check for error responses
    if (jsonObj.contains("error")) {
        QString errorType = jsonObj["error"].toString();
        qDebug() << "[Auth] Error response received:" << errorType;
        
        // Special handling during polling - "pending" or "waiting" errors mean keep polling
        if (pollTimer->isActive() && (errorType == "pending" || errorType == "waiting" || errorType == "not_authenticated")) {
            qDebug() << "[Auth] Still waiting for browser authentication...";
            return; // Continue polling
        }
        
        // Stop polling on actual errors
        if (pollTimer->isActive()) {
            stopPolling();
        }
        
        if (errorType.compare("Expired", Qt::CaseInsensitive) == 0) {
            qDebug() << "[Auth] Token expired detected";
            emit authenticationFailed("expired", QObject::tr("Token expired – login on www.sniffy.cz first"));
            return;
        } else if (errorType.compare("Login", Qt::CaseInsensitive) == 0) {
            emit authenticationFailed("not_logged-in", QObject::tr("Log in on www.sniffy.cz first and then click check login"));
            return;
        } else {
            emit authenticationFailed(errorType, QObject::tr("Authentication error: %1").arg(errorType));
            return;
        }
    }

    // Extract valid_till and token from JSON
    if (!jsonObj.contains("valid_till") || !jsonObj.contains("token")) {
        if (pollTimer->isActive()) {
            qDebug() << "[Auth] Missing fields during polling - continuing";
            return;
        }
        emit authenticationFailed("missing-fields", QObject::tr("Response missing required fields"));
        qDebug() << "[Auth] Missing valid_till or token in response";
        return;
    }

    const QString validityString = jsonObj["valid_till"].toString();
    const QString tokenString = jsonObj["token"].toString();
    
    if (validityString.isEmpty() || tokenString.isEmpty()) {
        if (pollTimer->isActive()) {
            return; // Continue polling
        }
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
        if (pollTimer->isActive()) {
            return; // Continue polling
        }
        emit authenticationFailed("invalid-response-format", QObject::tr("Invalid response format"));
        qDebug() << "[Auth] Invalid validity date or empty token";
        return;
    }

    // SUCCESS! Stop polling if we were polling
    if (pollTimer->isActive()) {
        stopPolling();
    }

    QDateTime previousValidity = CustomSettings::getTokenValidity();

    // Determine reconnection logic based on server context
    QString context = jsonObj["context"].toString();
    bool forceReconnect = false;
    
    if (context == "login") {
        forceReconnect = true;
    } else if (context == "demo") {
        // If we are already in demo mode, staying in demo doesn't require reconnect
        forceReconnect = !isDemoModeRequest;
    } else if (context == "extension") {
        // Reconnect if currently in demo mode (to upgrade to full)
        forceReconnect = isDemoModeRequest;
    }

    if (forceReconnect) {
        qDebug() << "[Auth] New Token generated: valid till" << validity << "(Context:" << context << ") - forcing device reconnection";
    } else {
        qDebug() << "[Auth] Token refreshed: valid till" << validity << "(Context:" << context << ") - no device reconnection";
    }
    
    CustomSettings::setLoginToken(token);
    CustomSettings::setTokenValidity(validity);
    CustomSettings::setTokenGeneratedDate(QDate::currentDate());
    CustomSettings::setLastLoginFailure("");
    CustomSettings::saveSettings();
    
    emit authenticationSucceeded(validity, token, forceReconnect); //if forceReconnect is true this reconnect the device. 
    if(!forceReconnect){
        // request small popup only on automatic prolongation
        const QString tip = QObject::tr("Login valid till %1")
                                .arg(validity.date().toString("dd.MM.yyyy"));
        emit popupMessageRequested(tip);
    }
}
