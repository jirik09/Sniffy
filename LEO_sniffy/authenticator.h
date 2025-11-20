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

public slots:
    // General login using provided email/pin hash
    void authenticate(const QString &email, const QString &pinHash);
    // Convenience token refresh using values from settings; optional device name and MCU ID
    void tokenRefresh(const QString &deviceName = QString(), const QString &mcuId = QString());

signals:
    void requestStarted();
    void requestFinished();
    void authenticationSucceeded(const QDateTime &validity, const QByteArray &token);
    void authenticationFailed(const QString &code, const QString &uiMessage);
    // Generic UI notification request; MainWindow can show a small popup/toast
    void popupMessageRequested(const QString &message);

private slots:
    void onFinished(QNetworkReply *reply);
    void onTimeout();

private:
    void startRequest(const QString &email, const QString &pinHash, const QString &deviceName, const QString &mcuId);

    QNetworkAccessManager *networkManager {nullptr};
    QPointer<QNetworkReply> currentReply;
    bool authenticationSent {false};
    QTimer *timeoutTimer {nullptr};
};

#endif // AUTHENTICATOR_H
