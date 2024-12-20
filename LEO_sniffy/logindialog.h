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
signals:
    void loginInfoChanged();

private slots:
    void buttonAction(int isCanceled);

public slots:
    void replyFinished(QNetworkReply*);

};

#endif // LOGINDIALOG_H
