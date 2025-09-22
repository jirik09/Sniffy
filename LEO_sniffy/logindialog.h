#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QtNetwork>
#include <QTimer>

#include "GUI/widgetbuttons.h"
#include "GUI/widgetselection.h"
#include "GUI/widgetseparator.h"
#include "GUI/widgettextinput.h"
#include "GUI/widgetlabel.h"
#include "customsettings.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void open();

private:
    Ui::LoginDialog *ui;
    WidgetTextInput  *userEmail;
    WidgetTextInput  *userPIN;
    WidgetLabel *info;

    WidgetButtons *buttonsDone;
    QNetworkAccessManager *networkManager {nullptr};
    /*
    "In-flight reply" refers to a response that has been requested but has not yet been received.
    The request is currently "in-flight", and the reply is pending. Purpose:
    - Timeout and retry logic: The system may wait for an in-flight reply, and if it doesn't arrive in time, it can retry.
    - Asynchronous architectures: In GUI or async RPC calls, some replies may not have arrived yet, while the program continues execution.
    */    
    bool requestInFlight {false};
    QNetworkReply *currentReply {nullptr};
    QTimer timeoutTimer; // 15s timeout for login requests

    // Helper methods to reduce duplication
    void reportFailure(const QString &uiMessage, const QString &failureCode, const QString &color = Graphics::palette().error);
    void startLoginNetworkRequest(const QString &email, const QString &pinHash);
    void finalizeSuccess(const QDateTime &validity, const QByteArray &token);

signals:
    void loginInfoChanged();
    void loginFailed(const QString &message);

private slots:
    void buttonAction(int isCanceled);

public slots:
    void replyFinished(QNetworkReply*);

};

#endif // LOGINDIALOG_H
