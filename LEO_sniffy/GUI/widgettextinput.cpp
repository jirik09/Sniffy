#include "widgettextinput.h"
#include "ui_widgettextinput.h"

WidgetTextInput::WidgetTextInput(QWidget *parent, QString name, QString value) :
    QWidget(parent),
    ui(new Ui::WidgetTextInput)
{
    ui->setupUi(this);
    ui->label->setText(name);
    setObjectName(name);
    ui->lineEdit->setText(value);

    ui->lineEdit->installEventFilter(this);
    ui->lineEdit->setStyleSheet(QString::fromUtf8(" QLineEdit{ background-color:")+BACKGROUND_COLOR_DATA_AREA+";}");
}

WidgetTextInput::~WidgetTextInput()
{
    delete ui;
}

QString WidgetTextInput::getText()
{
    return  ui->lineEdit->text();
}

bool WidgetTextInput::eventFilter(QObject *obj, QEvent *event){
    bool processTextEdit = false;
    if(event->type() == QEvent::FocusOut) processTextEdit = true;
    if(event->type() == QEnterEvent::KeyRelease){
        QKeyEvent *ev = (QKeyEvent*)event;
        if( (ev->key() == Qt::Key_Enter) || (ev->key() == Qt::Key_Return)) processTextEdit = true;
    }

    if(processTextEdit){
        emit textChanged(ui->lineEdit->text());
    }
    return QObject::eventFilter(obj, event);
}
