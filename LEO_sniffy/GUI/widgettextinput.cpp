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

    QString style = "QWidget:disabled{color: "+QString::fromUtf8(COLOR_GREY)+"} QWidget{color:"+QString::fromUtf8(COLOR_WHITE) +"}";
    ui->label->setStyleSheet(style);

    style = "QWidget:disabled{color: "+QString::fromUtf8(COLOR_GREY)+"} QWidget{background-color:"+BACKGROUND_COLOR_DATA_AREA+";color:"+QString::fromUtf8(COLOR_WHITE) +"}";
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

        QString input = ui->lineEdit->text();
        QString parse = input;
        QString suffix = input.right(1);

        qreal mult = 1;
        if(suffix == "u"){mult = 0.000001;parse = input.left(input.length()-1);}
        else if (suffix == "m"){mult = 0.001;parse = input.left(input.length()-1);}
        else if (suffix == "k"){mult = 1000;parse = input.left(input.length()-1);}
        else if (suffix == "M"){mult = 1000000;parse = input.left(input.length()-1);}

        bool success = false ;
        float flRead = QLocale().toFloat(parse,&success); //try local decimal separator
        if(!success){  //try . as a separator
            flRead = parse.toFloat(&success);
        }
        if (success){
            number = flRead * mult;
            emit numberChanged(number);
            lastParsed = input;
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

    if(event->type() == QEvent::FocusIn){
        ui->lineEdit->setStyleSheet(QString::fromUtf8(" QLineEdit{ background-color:")+BACKGROUND_COLOR_FOCUS_IN+";}");
    }

    if(processTextEdit){
        ui->lineEdit->setStyleSheet(QString::fromUtf8(" QLineEdit{ background-color:")+BACKGROUND_COLOR_DATA_AREA+";}");
        processInput();
    }
    return QObject::eventFilter(obj, event);
}
