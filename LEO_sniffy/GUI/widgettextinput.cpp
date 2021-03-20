#include "widgettextinput.h"
#include "ui_widgettextinput.h"

WidgetTextInput::WidgetTextInput(QWidget *parent, QString name, QString value, InputTextType type) :
    QWidget(parent),
    ui(new Ui::WidgetTextInput)
{
    ui->setupUi(this);
    ui->label->setText(name);
    setObjectName(name);
    ui->lineEdit->setText(value);

    ui->lineEdit->installEventFilter(this);
    ui->lineEdit->setStyleSheet(QString::fromUtf8(" QLineEdit{ background-color:")+BACKGROUND_COLOR_DATA_AREA+";}");
    inputType = type;
}

WidgetTextInput::~WidgetTextInput()
{
    delete ui;
}

QByteArray WidgetTextInput::saveGeometry()
{
    return getText().toUtf8();
}

void WidgetTextInput::restoreGeometry(QByteArray geom)
{
    ui->lineEdit->setText(geom);
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
