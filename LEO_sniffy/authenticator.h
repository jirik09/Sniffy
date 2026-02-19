#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QPointer>

class Authenticator : public QObject {
    Q_OBJECT
public:
    explicit Authenticator(QObject *parent = nullptr);

    void setDemoMode(bool demo);
public slots:
    // Check login status once (no polling) - used for initial check
    void checkLogin(const QString &email);
    // Start polling for authentication status. If immediate is true, checks immediately.
    void startPolling(bool immediate = true);
    // Stop polling
    void stopPolling();
    // Convenience token refresh using values from settings; optional device name and MCU ID; optional demo mode flag
    void tokenRefresh(const QString &deviceName = QString(), const QString &mcuId = QString(), bool isDemoMode = false);
    
    // Set the currently connected device info to be used in all requests
    void setConnectedDevice(const QString &name, const QString &id);

    // Get the persistent session ID
    QString getSessionId() const { return currentSessionId; }
    
    // Check if authentication was initiated manually by user
    bool isManualAuth() const { return authenticationSentManual; }

signals:
    void requestStarted();
    void requestFinished();
    void authenticationSucceeded(const QDateTime &validity, const QByteArray &token, bool forceReconnect);
    void authenticationFailed(const QString &code, const QString &uiMessage);
    void sessionStarted(const QString &sessionId); // Emitted when browser auth session is created
    // Generic UI notification request; MainWindow can show a small popup/toast
    void popupMessageRequested(const QString &message);

private slots:
    void onFinished(QNetworkReply *reply);
    void onTimeout();
    void checkAuthStatus(); // Polling slot for checking auth status

private:
    void startRequest(const QString &email, const QString &deviceName, const QString &mcuId);

    QNetworkAccessManager *networkManager {nullptr};
    QPointer<QNetworkReply> currentReply;
    bool authenticationSentManual {false};
    bool connectedInDemoMode {true}; // Track if currently in demo mode based on last request
    QTimer *timeoutTimer {nullptr};
    QTimer *pollTimer {nullptr}; // Timer for polling auth status
    QString currentSessionId; // Session ID for polling
    QString currentDeviceName;
    QString currentMcuId;
};

#endif // AUTHENTICATOR_H
