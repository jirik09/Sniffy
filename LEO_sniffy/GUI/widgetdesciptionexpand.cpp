#include "widgetdesciptionexpand.h"
#include "ui_widgetdesciptionexpand.h"

WidgetDesciptionExpand::WidgetDesciptionExpand(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::WidgetDesciptionExpand)
{
    ui->setupUi(this);

    ui->widget_sep->setText(name);
    labelsLayout = new QVBoxLayout();
    labelsLayout->setSpacing(0);
    labelsLayout->setContentsMargins(0,0,0,0);

    ui->widget_arrow->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"arrow_up.png);");
    ui->widget_sep->installEventFilter(this);
    ui->widget_arrow->installEventFilter(this);
}

WidgetDesciptionExpand::~WidgetDesciptionExpand()
{
    delete ui;
}

void WidgetDesciptionExpand::addLabel(QString name, QString value)
{
    if(!labelsLayout->parent()){
        ui->widget_info->setLayout(labelsLayout);
    }
    auto *lbl = new WidgetLabel(ui->widget_info,name,value);
    labels.append(lbl);
    labelsLayout->addWidget(lbl);
}

void WidgetDesciptionExpand::clearLabels()
{
    // Remove and delete child widgets explicitly
    for(auto *w : labels){
        if(w){
            w->setParent(nullptr);
            w->deleteLater();
        }
    }
    labels.clear();
    if(labelsLayout){
        // Leave layout in place; Qt will delete it with parent. Just remove its items.
        while(auto *item = labelsLayout->takeAt(0)){
            delete item; // deletes item wrapper, widget already scheduled for deletion
        }
    }
}

bool WidgetDesciptionExpand::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease){
        if(isHidden){
            isHidden = false;
            ui->widget_info->show();
            ui->widget_arrow->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"arrow_up.png);");
        }else{
            isHidden = true;
            ui->widget_info->hide();
            ui->widget_arrow->setStyleSheet("image: url("+Graphics::getGraphicsPath()+"arrow_down.png);");
        }
    }
    return QObject::eventFilter(obj, event);
}
