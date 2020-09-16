#include "widgetlist.h"
#include "ui_widgetlist.h"

WidgetList::WidgetList(QWidget *parent, int traceCount) :
    QWidget(parent),
    ui(new Ui::WidgetList), traceCount(traceCount)
{
    ui->setupUi(this);

    QStringListModel *model = new QStringListModel();
    ui->listView->setModel(model);

    QStringList list;
    list << "a" << "b" << "c" << "a" << "b" << "c" << "a" << "b" << "c"<< "a" << "b" << "c";
    model->setStringList(list);
}

WidgetList::~WidgetList()
{
    delete ui;
}
