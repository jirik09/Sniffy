#include "widgetdesciptionexpand.h"
#include "ui_widgetdesciptionexpand.h"

WidgetDesciptionExpand::WidgetDesciptionExpand(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetDesciptionExpand)
{
    ui->setupUi(this);

    ui->widget_sep->setText(name);
    labels = new QList<WidgetLabel *>;

    labelsLayout = new QVBoxLayout();
    labelsLayout->setSpacing(0);
    labelsLayout->setContentsMargins(0,0,0,0);

    ui->widget_info->setLayout(labelsLayout);
}

WidgetDesciptionExpand::~WidgetDesciptionExpand()
{
    delete ui;
}

void WidgetDesciptionExpand::addLabel(QString name, QString value)
{
    WidgetLabel *lbl = new WidgetLabel(ui->widget_info,name,value);
    labels->append(lbl);
    labelsLayout->addWidget(lbl);
    ui->widget_info->setLayout(labelsLayout);
}

void WidgetDesciptionExpand::clearLabels()
{
    labelsLayout->deleteLater();
    labels->clear();
    labelsLayout = new QVBoxLayout();
}
