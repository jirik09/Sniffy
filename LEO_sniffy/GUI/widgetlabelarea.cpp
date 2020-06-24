#include "widgetlabelarea.h"
#include "ui_widgetlabelarea.h"

widgetLabelArea::widgetLabelArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetLabelArea)
{
    ui->setupUi(this);
    ui->widget_Hcursors->hide();
    ui->widget_Vcursors->hide();
  //  ui->widget_meas->hide();
  //  ui->widget_measAndCursors->hide();

   // labelList = new QList<QLabel*>();

    labelList.append(ui->label_meas1);
    labelList.append(ui->label_meas2);
    labelList.append(ui->label_meas3);
    labelList.append(ui->label_meas4);
    labelList.append(ui->label_meas5);
    labelList.append(ui->label_meas6);
    labelList.append(ui->label_meas7);
    labelList.append(ui->label_meas8);
    labelList.append(ui->label_meas9);
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

void widgetLabelArea::setMeasurements(QList<Measurement*> meas){
    int fillIndex = 0;
    foreach(Measurement* m, meas){
        labelList.at(fillIndex)->setText(m->getLabel()+m->getValue());
        fillIndex++;
    }
}

