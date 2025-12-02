#ifndef FIRMWAREMANAGER_H
#define FIRMWAREMANAGER_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QColor>
#include <QFile>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDateTime>

#include "flasher/stlinkflasher.h"
#include "authenticator.h"

class FirmwareManager : public QObject
{
    Q_OBJECT
public:
    explicit FirmwareManager(QObject *parent = nullptr);
    ~FirmwareManager();

    void startUpdateProcess();
    bool isFlashInProgress() const;

signals:
    void progressChanged(int value, int total);
    void statusMessage(const QString &msg, const QColor &color);
    void logMessage(const QString &msg);
    void operationStarted();
    void operationFinished(bool success);
    void firmwareFlashed();

private slots:
    // Flasher slots
    void onDeviceConnected(const QString &info);
    void onFlashProgress(int value, int total);
    void onFlashLog(const QString &msg);
    void onFlashFinished(bool success, const QString &msg);
    void onOperationStarted(const QString &operation);
    void onDeviceUIDAvailable(const QString &uidHex);
    void onDeviceUIDError(const QString &message);

    // Auth slots
    void onAuthStarted();
    void onAuthFinished();
    void onAuthFailed(const QString &code, const QString &uiMessage);
    void onAuthSucceeded(const QDateTime &validity, const QByteArray &token);

    // Network slots
    void onFirmwareDownloadFinished(QNetworkReply *reply);

private:
    void failOperation(const QString &msg);

    StLinkFlasher *m_flasher;
    QThread *m_flasherThread;
    Authenticator *m_auth;
    QNetworkAccessManager *m_networkManager;

    bool m_flashInProgress;
    QString m_lastReadUidHex;
};

#endif // FIRMWAREMANAGER_H
