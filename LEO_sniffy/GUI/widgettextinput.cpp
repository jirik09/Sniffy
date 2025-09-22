#include "widgettextinput.h"
#include "ui_widgettextinput.h"
#include "stylehelper.h"

WidgetTextInput::WidgetTextInput(QWidget *parent, QString name, QString value, InputTextType type) :
    QWidget(parent),
    ui(new Ui::WidgetTextInput)
{
    ui->setupUi(this);
    ui->label->setText(name);
    setObjectName(name);
    ui->lineEdit->setText(value);

    ui->lineEdit->installEventFilter(this);
    inputType = type;

    /* Initialize lastParsed only if NUMBER and initial value parses, 
    preventing fallback inserting a default digit on first focus loss. */
    if(inputType == InputTextType::NUMBER){
        bool ok=false; qreal v = NumberParser::parse(value,ok);
        if(ok){
            number = v;
            lastParsed = value;
        }else{
            lastParsed.clear();
        }
    }

    ui->label->setStyleSheet(StyleHelper::textInputLabel());
    ui->lineEdit->setStyleSheet(StyleHelper::textInputField());
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
    if(geom.isEmpty()) return;
    ui->lineEdit->setText(geom);
}

QString WidgetTextInput::getText()
{
    return  ui->lineEdit->text();
}

qreal WidgetTextInput::getValue()
{
    return number;
}

void WidgetTextInput::processInput()
{
    if(inputType == InputTextType::STRING){
        emit textChanged(ui->lineEdit->text());
    }else if(inputType == InputTextType::NUMBER){
        bool success = false ;
        QString current = ui->lineEdit->text();
        qreal value = NumberParser::parse(current,success);
        if (success){
            number = value;
            emit numberChanged(number);
            lastParsed = current;
        }else{
            // If we have no lastParsed yet, keep field empty rather than forcing a placeholder digit.
            if(!lastParsed.isEmpty()){
                ui->lineEdit->setText(lastParsed);
            }else{
                ui->lineEdit->clear();
            }
        }
    }
}

void WidgetTextInput::setAsPassword()
{
    ui->lineEdit->setEchoMode(QLineEdit::Password);
}

void WidgetTextInput::setText(QString txt)
{
    ui->lineEdit->setText(txt);
}

bool WidgetTextInput::eventFilter(QObject *obj, QEvent *event){
    bool processTextEdit = false;
    if(event->type()==QEvent::Show)graphicsShown = true;
    if(event->type()==QEvent::Hide)graphicsShown = false;

    if(graphicsShown && event->type() == QEvent::FocusOut) processTextEdit = true;
    if(graphicsShown && event->type() == QEvent::KeyRelease){
        auto *ev = static_cast<QKeyEvent*>(event);
        if(ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
            processTextEdit = true;
    }

    if(processTextEdit){
        processInput();
    }
    return QObject::eventFilter(obj, event);
}
