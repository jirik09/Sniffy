#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QTimer>

#include "GUI/widgetbuttons.h"
#include "GUI/widgetselection.h"
#include "GUI/widgetseparator.h"
#include "GUI/widgettextinput.h"
#include "GUI/widgetlabel.h"
#include "customsettings.h"
#include "GUI/clickablelabel.h"

// Forward declaration at namespace scope
class Authenticator;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(Authenticator *authenticator, QWidget *parent = nullptr);
    ~LoginDialog();
    void open();

private:
    Ui::LoginDialog *ui;
    WidgetTextInput  *userEmail;
    WidgetTextInput  *userPIN;
    WidgetLabel *info;

    WidgetButtons *buttonsDone;
    ClickableLabel *logoutLabel{nullptr};
    
    // Shared authenticator for login
    Authenticator *auth {nullptr};

    // Helper methods to reduce duplication
    void reportFailure(const QString &uiMessage, const QString &failureCode, const QString &color = Graphics::palette().error);
    void startLoginNetworkRequest(const QString &email, const QString &pinHash);
    void finalizeSuccess(const QDateTime &validity, const QByteArray &token);

    void performLogout();

signals:
    void loginInfoChanged();
    void loginFailed(const QString &message);

private slots:
    void buttonAction(int isCanceled);

public slots:
    // No direct network reply handling; moved to Authenticator

};

#endif // LOGINDIALOG_H
