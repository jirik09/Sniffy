#include "widgetlabelarea.h"
#include "ui_widgetlabelarea.h"

WidgetLabelArea::WidgetLabelArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetLabelArea)
{
    ui->setupUi(this);
    ui->widget_Hcursors->hide();
    ui->widget_Vcursors->hide();
    ui->widget_meas->hide();
    ui->widget_measAndCursors->hide();

   // labelList = new QList<QLabel*>();

    measLabelList.append(ui->label_meas1);
    measLabelList.append(ui->label_meas2);
    measLabelList.append(ui->label_meas3);
    measLabelList.append(ui->label_meas4);
    measLabelList.append(ui->label_meas5);
    measLabelList.append(ui->label_meas6);
    measLabelList.append(ui->label_meas7);
    measLabelList.append(ui->label_meas8);
    measLabelList.append(ui->label_meas9);

    foreach(QLabel *label, measLabelList){
        label->hide();
    }

    channLabelList.append(ui->label_ch1);
    channLabelList.append(ui->label_ch2);
    channLabelList.append(ui->label_ch3);
    channLabelList.append(ui->label_ch4);
    channLabelList.append(ui->label_math1);
    channLabelList.append(ui->label_math2);

    ui->label_ch1->setStyleSheet(QString::fromUtf8(TEXT_COLOR_ORANGE));
    ui->label_ch2->setStyleSheet(QString::fromUtf8(TEXT_COLOR_BLUE));
    ui->label_ch3->setStyleSheet(QString::fromUtf8(TEXT_COLOR_GREEN));
    ui->label_ch4->setStyleSheet(QString::fromUtf8(TEXT_COLOR_PURPLE));

    foreach(QLabel *label, channLabelList){
        label->hide();
    }
}

WidgetLabelArea::~WidgetLabelArea()
{
    delete ui;
}

void WidgetLabelArea::setChannelLabelVisible(int channelIndex, bool isVisible){
    if(isVisible){
        channLabelList[channelIndex]->show();
    }else{
        channLabelList[channelIndex]->hide();
    }
}

void WidgetLabelArea::hideChannelLabels(){
    foreach(QLabel *label, channLabelList){
        label->hide();
    }
}

void WidgetLabelArea::setTriggerLabelText(QString text){
    ui->label_trigger->setText(text);
}

void WidgetLabelArea::setSamplingRateLabelText(QString text){
    ui->label_sampling->setText(text);
}

void WidgetLabelArea::setScaleLabelText(QString text){
    ui->label_scale->setText(text);
}

void WidgetLabelArea::setMeasurements(QList<Measurement*> meas){
    int fillIndex = 0;

    if(meas.length()==0){
        ui->widget_meas->hide();
        ui->widget_measAndCursors->hide();
        foreach(QLabel *label, measLabelList){
            label->hide();
        }
    }else{
        ui->widget_meas->show();
        ui->widget_measAndCursors->show();
    }

    foreach(Measurement* m, meas){
        measLabelList.at(fillIndex)->setText(m->getLabel()+" "+m->getValue());
        measLabelList.at(fillIndex)->show();
        if(m->getChannelIndex()==0)
            measLabelList.at(fillIndex)->setStyleSheet(QString::fromUtf8(TEXT_COLOR_ORANGE));
        if(m->getChannelIndex()==1)
            measLabelList.at(fillIndex)->setStyleSheet(QString::fromUtf8(TEXT_COLOR_BLUE));
        if(m->getChannelIndex()==2)
            measLabelList.at(fillIndex)->setStyleSheet(QString::fromUtf8(TEXT_COLOR_GREEN));
        if(m->getChannelIndex()==3)
            measLabelList.at(fillIndex)->setStyleSheet(QString::fromUtf8(TEXT_COLOR_PURPLE));
        fillIndex++;
    }
}

