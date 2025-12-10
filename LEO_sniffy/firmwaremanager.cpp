#include "firmwaremanager.h"
#include "graphics/graphics.h"
#include "customsettings.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>

FirmwareManager::FirmwareManager(Authenticator *auth, QObject *parent) : QObject(parent),
                                                    m_flashInProgress(false)
{
    // Initialize Flasher
    m_flasher = new StLinkFlasher();
    m_flasherThread = new QThread(this);
    m_flasher->moveToThread(m_flasherThread);
    m_flasherThread->start();

    connect(m_flasher, &StLinkFlasher::progressChanged, this, &FirmwareManager::onFlashProgress);
    connect(m_flasher, &StLinkFlasher::logMessage, this, &FirmwareManager::onFlashLog);
    connect(m_flasher, &StLinkFlasher::operationFinished, this, &FirmwareManager::onFlashFinished);
    connect(m_flasher, &StLinkFlasher::deviceConnected, this, &FirmwareManager::onDeviceConnected);
    connect(m_flasher, &StLinkFlasher::operationStarted, this, &FirmwareManager::onOperationStarted);
    connect(m_flasher, &StLinkFlasher::deviceUIDAvailable, this, &FirmwareManager::onDeviceUIDAvailable);
    connect(m_flasher, &StLinkFlasher::deviceUIDError, this, &FirmwareManager::onDeviceUIDError);

    // Authenticator for remote flow
    m_auth = auth;
    connect(m_auth, &Authenticator::requestStarted, this, &FirmwareManager::onAuthStarted);
    connect(m_auth, &Authenticator::requestFinished, this, &FirmwareManager::onAuthFinished);
    connect(m_auth, &Authenticator::authenticationFailed, this, &FirmwareManager::onAuthFailed);
    connect(m_auth, &Authenticator::authenticationSucceeded, this, &FirmwareManager::onAuthSucceeded);

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &FirmwareManager::onFirmwareDownloadFinished);
}

FirmwareManager::~FirmwareManager()
{
    if (m_flasher)
    {
        // Invoke cleanup in the flasher's thread
        QMetaObject::invokeMethod(m_flasher, "stopAndCleanup");
    }
    else
    {
        m_flasherThread->quit();
    }

    // Wait for the thread to finish with timeout
    if (!m_flasherThread->wait(5000))
    {
        m_flasherThread->terminate();
        m_flasherThread->wait();
    }

    delete m_flasher;
    m_flasher = nullptr;
    // m_auth and m_networkManager are children, deleted automatically
}

void FirmwareManager::startUpdateProcess()
{
    if (m_flashInProgress)
    {
        emit statusMessage("Flash already in progress...", Graphics::palette().warning, MsgError);
        return;
    }

    emit operationStarted();
    emit statusMessage("Connecting to ST-Link...", Graphics::palette().textAll, MsgInfo);
    emit progressChanged(0, 100);

    // Trigger connection in the thread
    QMetaObject::invokeMethod(m_flasher, "connectDevice");
}

bool FirmwareManager::isFlashInProgress() const
{
    return m_flashInProgress;
}

void FirmwareManager::onDeviceConnected(const QString &info)
{
    emit statusMessage("Reading MCU ID...", Graphics::palette().textAll, MsgInfo);
    QMetaObject::invokeMethod(m_flasher, "readDeviceUID");
}

void FirmwareManager::onFlashProgress(int value, int total)
{
    emit progressChanged(value, total);
}

void FirmwareManager::onFlashLog(const QString &msg)
{
    // Only update if operation is in progress to prevent overwriting final result/error
    emit statusMessage(msg, Graphics::palette().textAll, MsgInfo);
}

void FirmwareManager::onFlashFinished(bool success, const QString &msg)
{
    emit statusMessage(msg, success ? Graphics::palette().running : Graphics::palette().error, success ? MsgSuccess : MsgError);

    if (success)
    {
        emit progressChanged(100, 100);
        emit firmwareFlashed();
    }

    m_flashInProgress = false;
    emit operationFinished(success);

    // Disconnect after operation
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}

void FirmwareManager::onOperationStarted(const QString &operation)
{
    Q_UNUSED(operation);
    m_flashInProgress = true;
    emit operationStarted();
}

void FirmwareManager::onDeviceUIDAvailable(const QString &uidHex, const QString &mcu)
{
    m_lastReadUidHex = uidHex;

    // Check local
    QString appDir = QCoreApplication::applicationDirPath();
    QString localBinPath = appDir + "/" + uidHex + ".bin";

    if (QFile::exists(localBinPath))
    {
        emit statusMessage("Local firmware found. Flashing...", Graphics::palette().running, MsgInfo);
        QMetaObject::invokeMethod(m_flasher, "flashFirmware", Q_ARG(QString, localBinPath));
        return;
    }

    // Construct URL dynamically
    QString email = CustomSettings::getUserEmail();
    // Use validity timestamp as session ID (e.g. 1733227200)
    QString sessionId = QString::number(CustomSettings::getTokenValidity().toSecsSinceEpoch());

    if (email.isEmpty() || !CustomSettings::hasValidLogin())
    {
        failOperation("Unknown user or invalid session. Please log in.", MsgLoginRequired);
        return;
    }

    emit statusMessage("Requesting remote firmware...", Graphics::palette().running, MsgInfo);

    QUrl url("https://sniffy.cz/scripts/sniffy_bin_req.php?");
    QUrlQuery query;
    query.addQueryItem("email", email);
    query.addQueryItem("session_ID", sessionId);
    query.addQueryItem("MCU_UID", uidHex);
    query.addQueryItem("MCU", mcu);
    url.setQuery(query);

    QNetworkRequest request(url);
    m_networkManager->get(request);
}

void FirmwareManager::onDeviceUIDError(const QString &message)
{
    failOperation("Failed to read MCU ID: " + message);
}

void FirmwareManager::onAuthStarted()
{
    // Keep UI busy; already disabled by onOperationStarted
}

void FirmwareManager::onAuthFinished()
{
    // No-op; wait for success/fail signals
}

void FirmwareManager::onAuthFailed(const QString &code, const QString &uiMessage)
{
    Q_UNUSED(code);
    failOperation(uiMessage);
}

void FirmwareManager::onAuthSucceeded(const QDateTime &validity, const QByteArray &token)
{
    Q_UNUSED(validity);
    Q_UNUSED(token);

    emit statusMessage("Remote auth OK. (Placeholder flow)", Graphics::palette().running, MsgSuccess);

    // End operation cleanly for now
    m_flashInProgress = false;
    emit operationFinished(true);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}

void FirmwareManager::onFirmwareDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        failOperation("Download Error");
        reply->deleteLater();
        return;
    }

    // Extract filename from Content-Disposition
    QString filename;
    QVariant contentDisp = reply->header(QNetworkRequest::ContentDispositionHeader);
    QString contentDispStr = contentDisp.toString();
    if (!contentDispStr.isEmpty())
    {
        int nameIdx = contentDispStr.indexOf("filename=");
        if (nameIdx != -1)
        {
            filename = contentDispStr.mid(nameIdx + 9);
            // Remove quotes and semicolons if present
            filename = filename.split(';').first();
            filename = filename.replace("\"", "").trimmed();
        }
    }

    // Fallback to URL filename if header is missing
    if (filename.isEmpty())
    {
        filename = reply->url().fileName();
    }

    // Fallback if still empty
    if (filename.isEmpty())
    {
        filename = "unknown_firmware.bin";
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    if (data.isEmpty())
    {
        failOperation("Error: Empty response from server.");
        return;
    }

    // Check for JSON error response
    // Simple check: if it starts with { and contains "error"
    if (data.trimmed().startsWith('{'))
    {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull() && doc.isObject())
        {
            QJsonObject obj = doc.object();
            if (obj.contains("error"))
            {
                QString errorMsg = obj["error"].toString();
                if (errorMsg.length() > 150) {
                    errorMsg = errorMsg.left(150) + "...";
                }
                failOperation("Server reply: " + errorMsg);
                return;
            }
        }
    }

    // Save to file with the ORIGINAL filename
    QString appDir = QCoreApplication::applicationDirPath();
    QString filePath = appDir + "/" + filename;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        failOperation("Error: Could not save firmware file: " + filename);
        return;
    }
    file.write(data);
    file.close();

    // Check if filename matches UID
    QString expectedName = m_lastReadUidHex + ".bin";
    if (filename.compare(expectedName, Qt::CaseInsensitive) != 0)
    {
        failOperation("Error: Downloaded file (" + filename + ") does not match MCU UID.");
        return;
    }

    emit statusMessage("Download complete. Flashing...", Graphics::palette().textAll, MsgInfo);

    // Now flash it
    QMetaObject::invokeMethod(m_flasher, "flashFirmware", Q_ARG(QString, filePath));
}

void FirmwareManager::failOperation(const QString &msg, int msgType)
{
    emit statusMessage(msg, Graphics::palette().error, msgType);
    m_flashInProgress = false;
    emit operationFinished(false);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}
