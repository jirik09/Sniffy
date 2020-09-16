#include "widgetlist.h"
#include "ui_widgetlist.h"

WidgetList::WidgetList(QWidget *parent, int traceCount) :
    QWidget(parent),
    ui(new Ui::WidgetList)
{
    ui->setupUi(this);

    model = new QStringListModel();
    ui->listView->setModel(model);
    ui->listView->setStyleSheet("alternate-background-color: rgb(58, 58, 58);");

    lists.resize(traceCount);
    for(int i = 0; i < traceCount; i++)
        model->setStringList(lists[i]);
}

WidgetList::~WidgetList()
{
    delete ui;
}

void WidgetList::appendNumber(int listIndex, float timeMark, float num){
    lists[listIndex].prepend(QString::number(timeMark, 'f', 1) + "; " + QString::number(num));
    model->setStringList(lists[listIndex]);
}

void WidgetList::appendString(int listIndex, QString timeMark, QString str){
    lists[listIndex].prepend(timeMark + "; " + str);
    model->setStringList(lists[listIndex]);
}

void WidgetList::clearSample(int index){
    for(int i = 0; i < lists.length(); i++){
        lists[i].removeAt(index);
        model->setStringList(lists[i]);
    }
}

void WidgetList::clearLast(){
    for(int i = 0; i < lists.length(); i++){
        lists[i].removeLast();
        model->removeRow(0);
        model->setStringList(lists[i]);
    }
}
