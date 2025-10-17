#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QPointer>

class Authenticator : public QObject {
    Q_OBJECT
public:
    explicit Authenticator(QObject *parent = nullptr);

public slots:
    // General login using provided email/pin hash; optional device name
    void authenticate(const QString &email, const QString &pinHash, const QString &deviceName = QString());
    // Convenience refresh using values from settings; optional device name
    void refresh(const QString &deviceName = QString());

signals:
    void requestStarted();
    void requestFinished();
    void authenticationSucceeded(const QDateTime &validity, const QByteArray &token);
    void authenticationFailed(const QString &code, const QString &uiMessage);

private slots:
    void onFinished(QNetworkReply *reply);
    void onTimeout();

private:
    void startRequest(const QString &email, const QString &pinHash, const QString &deviceName);

    QNetworkAccessManager *networkManager {nullptr};
    QPointer<QNetworkReply> currentReply;
    QTimer *timeoutTimer {nullptr};
};

#endif // AUTHENTICATOR_H
