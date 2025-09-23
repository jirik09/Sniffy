#include "widgetlist.h"
#include "ui_widgetlist.h"
#include "../diagnostics.h"

WidgetList::WidgetList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetList)
{
    ui->setupUi(this);

    model = new QStringListModel(this);
    ui->listView->setModel(model);
    model->setStringList(list);
}

WidgetList::~WidgetList()
{
    delete ui;
}

void WidgetList::appendNumber(float timeMark, QString prefix, float num, QString affix){
    QString entry = QString::number(timeMark, 'f', 1) + ": " + prefix + QString::number(num) + affix;
    list.prepend(entry);
    // Incremental update instead of resetting entire list
    model->insertRow(0);
    model->setData(model->index(0), entry);
}

void WidgetList::appendString(QString timeMark, QString str, QString affix){
    QString entry = timeMark + ": " + str + affix;
    list.prepend(entry);
    model->insertRow(0);
    model->setData(model->index(0), entry);
}

void WidgetList::associateNumber(QString prefix, float num, QString affix){
    if(list.isEmpty()){
        SNIFFY_LOG_WARN("associateNumber ignored (empty list)");
        return;
    }
    QString newStr = list.first() + prefix + QString::number(num, 'g') + affix;
    list[0] = newStr;
    model->setData(model->index(0), newStr);
}

void WidgetList::associateString(QString prefix, QString num, QString affix){
    if(list.isEmpty()){
        SNIFFY_LOG_WARN("associateString ignored (empty list)");
        return;
    }
    QString newStr = list.first() + prefix + num + affix;
    list[0] = newStr;
    model->setData(model->index(0), newStr);
}

void WidgetList::appendAssociatedString(QString str){
    if(list.isEmpty()){
        SNIFFY_LOG_WARN("appendAssociatedString ignored (empty list)");
        return;
    }
    QString newStr = list.first() + str;
    list[0] = newStr;
    model->setData(model->index(0), newStr);
}

void WidgetList::clearSample(int index){
    if(index < 0 || index >= list.size()){
        SNIFFY_LOG_WARN("clearSample index out of range:" << index);
        return;
    }
    list.removeAt(index);
    model->removeRow(index);
}

void WidgetList::clearLast(){
    if(list.isEmpty()){
        SNIFFY_LOG_INFO("clearLast no-op: list empty");
        return;
    }
    list.removeLast();
    model->removeRow(model->rowCount()-1);
}

void WidgetList::clear(){
    model->removeRows(0, model->rowCount());
    list.clear();
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
