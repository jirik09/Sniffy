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
    inputType = type;

    QString style = Graphics::STYLE_TEXTINPUT;
    ui->label->setStyleSheet(style);

    style = Graphics::STYLE_TEXTINPUT+"QWidget{background-color:"+Graphics::COLOR_DATA_AREA+"} QWidget::focus{ background-color:"+Graphics::COLOR_BACKGROUND_FOCUS_IN+";}";
    ui->lineEdit->setStyleSheet(style);
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
        qreal value = NumberParser::parse(ui->lineEdit->text(),success);
        if (success){
            number = value;
            emit numberChanged(number);
            lastParsed = ui->lineEdit->text();
        }else{
            ui->lineEdit->setText(lastParsed);
        }
    }
}

bool WidgetTextInput::eventFilter(QObject *obj, QEvent *event){
    bool processTextEdit = false;
    if(event->type() == QEvent::FocusOut) processTextEdit = true;
    if(event->type() == QEnterEvent::KeyRelease){
        QKeyEvent *ev = (QKeyEvent*)event;
        if( (ev->key() == Qt::Key_Enter) || (ev->key() == Qt::Key_Return)) processTextEdit = true;
    }

    if(processTextEdit){
        processInput();
    }
    return QObject::eventFilter(obj, event);
}
