#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Login");

    setStyleSheet("QWidget{background-color:"+Graphics::COLOR_WINDOW_WIDGET+";}");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(2);
    layout->setContentsMargins(5,5,5,5);


    userEmail= new WidgetTextInput(this,"User email  ");
    userPIN = new WidgetTextInput(this, "PIN  ","",InputTextType::NUMBER);
    userPIN->setAsPassword();
    layout->addWidget(userEmail);
    layout->addWidget(userPIN);
    WidgetSeparator *sep1 = new WidgetSeparator(this,"");
    layout->addWidget(sep1);
    info = new WidgetLabel(this,"First time here? register at www.sniffy.cz"); ////TODO create this as link
    layout->addWidget(info);

    QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer);

    WidgetSeparator *sep2 = new WidgetSeparator(this);
    layout->addWidget(sep2);

    buttonsDone = new WidgetButtons(this,2);
    buttonsDone->setText("  Login  ",0);
    buttonsDone->setText(" Close ",1);
    layout->addWidget(buttonsDone);

    ui->widgetLogin->setLayout(layout);

    userEmail->setText(CustomSettings::getUserEmail());

    connect(buttonsDone,&WidgetButtons::clicked,this,&LoginDialog::buttonAction);


}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::open()
{
    //  infoLabel->setValue("");
    //  infoLabel->setColor(Graphics::COLOR_TEXT_ALL);
    this->show();
    this->raise();
    this->activateWindow();
}

void LoginDialog::buttonAction(int isCanceled)
{
    if(!isCanceled){
        CustomSettings::setUserEmail(userEmail->getText());
        QString pinHash = QString(QCryptographicHash::hash(QString::number(userPIN->getValue()+1297).toUtf8(),QCryptographicHash::Sha1).toHex());
        CustomSettings::setUserPin(pinHash);
        CustomSettings::saveSettings();

        QUrl auth = QUrl("http://sniffy.cz/sniffy_auth.php?email="+CustomSettings::getUserEmail()+"&pin="+CustomSettings::getUserPin());

        QNetworkAccessManager *m_manager = new QNetworkAccessManager(this);
        connect(m_manager, &QNetworkAccessManager::finished,this, &LoginDialog::replyFinished);m_manager->get(QNetworkRequest(auth));

        info->setName("Verification");
        info->setColor(Graphics::COLOR_TEXT_LABEL);
        emit loginInfoChanged();

    }else{
       this->close();
    }
}

void LoginDialog::replyFinished(QNetworkReply *pReply)
{
    QByteArray data = (pReply->readAll());


    if(data =="Expired"){
        info->setName("Token has expired please login on www.sniffy.cz");
        info->setColor(Graphics::COLOR_WARNING);
    }else if(data =="wrongPin"){
        info->setName("Wrong pin or email");
        info->setColor(Graphics::COLOR_ERROR);
    }else{
        QString validity(data.left(19));
        QByteArray token = (data.mid(19,256));
        CustomSettings::setLoginToken(token);
        CustomSettings::setTokenValidity(QDateTime::fromString(validity,"yyyy-MM-dd hh:mm:ss"));
        CustomSettings::saveSettings();
        info->setName("Login valid till: " + CustomSettings::getTokenValidity().toString("dd.MM.yyyy hh:mm"));
        info->setColor(Graphics::COLOR_TEXT_ALL);
        emit loginInfoChanged();
    }

}
