#include "firmwaremanager.h"
#include "graphics/graphics.h"
#include <QDebug>

FirmwareManager::FirmwareManager(QObject *parent) : QObject(parent),
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
    m_auth = new Authenticator(this);
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
        emit statusMessage("Flash already in progress...", Graphics::palette().warning);
        return;
    }

    emit operationStarted();
    emit statusMessage("Connecting to ST-Link...", Graphics::palette().textAll);
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
    emit statusMessage("Connected: " + info + ". Reading MCU ID...", Graphics::palette().textAll);
    QMetaObject::invokeMethod(m_flasher, "readDeviceUID");
}

void FirmwareManager::onFlashProgress(int value, int total)
{
    emit progressChanged(value, total);
}

void FirmwareManager::onFlashLog(const QString &msg)
{
    emit logMessage(msg);
    // Optionally update label with short status
    if (msg.length() < 50)
        emit statusMessage(msg, Graphics::palette().textAll);
}

void FirmwareManager::onFlashFinished(bool success, const QString &msg)
{
    emit statusMessage(msg, success ? Graphics::palette().running : Graphics::palette().error);

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

void FirmwareManager::onDeviceUIDAvailable(const QString &uidHex)
{
    m_lastReadUidHex = uidHex;

    // Check local
    QString appDir = QCoreApplication::applicationDirPath();
    QString localBinPath = appDir + "/" + uidHex + ".bin";

    if (QFile::exists(localBinPath))
    {
        emit statusMessage("Local firmware found (" + uidHex + ".bin). Flashing...", Graphics::palette().running);
        QMetaObject::invokeMethod(m_flasher, "flashFirmware", Q_ARG(QString, localBinPath));
        return;
    }

    emit statusMessage("Local firmware not found. Requesting remote...", Graphics::palette().textAll);

    // Temporary URL -> fix later with real endpoint
    // Using the test URL structure but injecting the actual UID
    QString urlStr = "https://sniffy.cz/firmware/<uid_hex>.bin";

    QNetworkRequest request((QUrl(urlStr)));
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

    emit statusMessage("Remote auth OK. (Placeholder flow)", Graphics::palette().running);

    // End operation cleanly for now
    m_flashInProgress = false;
    emit operationFinished(true);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}

void FirmwareManager::onFirmwareDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        failOperation("Download Error: " + reply->errorString());
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

    emit statusMessage("Downloaded " + filename + ". Flashing...", Graphics::palette().textAll);

    // Now flash it
    QMetaObject::invokeMethod(m_flasher, "flashFirmware", Q_ARG(QString, filePath));
}

void FirmwareManager::failOperation(const QString &msg)
{
    emit statusMessage(msg, Graphics::palette().error);
    m_flashInProgress = false;
    emit operationFinished(false);
    QMetaObject::invokeMethod(m_flasher, "disconnectDevice");
}
