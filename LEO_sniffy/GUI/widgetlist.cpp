#include "widgetlist.h"
#include "ui_widgetlist.h"

WidgetList::WidgetList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetList)
{
    ui->setupUi(this);

    model = new QStringListModel();
    ui->listView->setModel(model);
    ui->listView->setStyleSheet("alternate-background-color: "+Graphics::getControlBackgroundColor());
    model->setStringList(list);
}

WidgetList::~WidgetList()
{
    delete ui;
}

void WidgetList::appendNumber(float timeMark, QString prefix, float num, QString affix){
    list.prepend(QString::number(timeMark, 'f', 1) + ": " + prefix + QString::number(num) + affix);
    model->setStringList(list);
}

void WidgetList::appendString(QString timeMark, QString str, QString affix){
    list.prepend(timeMark + ": " + str + affix);
    model->setStringList(list);
}

void WidgetList::associateNumber(QString prefix, float num, QString affix){
    QString newStr = list.first() + prefix + QString::number(num, 'g') + affix;
    list.removeFirst();
    list.prepend(newStr);
    model->setStringList(list);
}

void WidgetList::associateString(QString prefix, QString num, QString affix){
    QString newStr = list.first() + prefix + num + affix;
    list.removeFirst();
    list.prepend(newStr);
    model->setStringList(list);
}

void WidgetList::appendAssociatedString(QString str){
    QString newStr = list.first() + str;
    list.removeFirst();
    list.prepend(newStr);
    model->setStringList(list);
}

void WidgetList::clearSample(int index){
    list.removeAt(index);
    model->setStringList(list);
}

void WidgetList::clearLast(){
    list.removeLast();
    model->removeRow(0);
    model->setStringList(list);
}

void WidgetList::clear(){
    model->removeRows(0, model->rowCount());
    list.clear();
    model->setStringList(list);
}

void WidgetList::saveList(QString name){
    QFile log("log_" + name + ".txt");
    if (log.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&log);
        for(int i = 0; i < list.length(); i++){
            out << list.at(i) + "\n";
        }
    }
}
