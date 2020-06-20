#include "widgetlabelarea.h"
#include "ui_widgetlabelarea.h"

widgetLabelArea::widgetLabelArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetLabelArea)
{
    ui->setupUi(this);
    ui->widget_Hcursors->hide();
    ui->widget_Vcursors->hide();
    ui->widget_meas->hide();
    ui->widget_measAndCursors->hide();
}

widgetLabelArea::~widgetLabelArea()
{
    delete ui;
}


void widgetLabelArea::setTriggerLabelText(QString text){
    ui->label_trigger->setText(text);
}

void widgetLabelArea::setSamplingRateLabelText(QString text){
    ui->label_sampling->setText(text);
}

void widgetLabelArea::setScaleLabelText(QString text){
    ui->label_scale->setText(text);
}

