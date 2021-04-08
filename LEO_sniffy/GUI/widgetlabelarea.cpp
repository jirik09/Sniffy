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

    ui->label_ch1->setStyleSheet("color:"+Colors::getChannelColor(0));
    ui->label_ch2->setStyleSheet("color:"+Colors::getChannelColor(1));
    ui->label_ch3->setStyleSheet("color:"+Colors::getChannelColor(2));
    ui->label_ch4->setStyleSheet("color:"+Colors::getChannelColor(3));
    ui->label_math1->setStyleSheet("color:"+Colors::getChannelColor(4));
    ui->label_math2->setStyleSheet("color:"+Colors::getChannelColor(5));

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

void WidgetLabelArea::setChannelScale(int index, QString Scale){
    int ch = index +1;
    if(index>3){
        channLabelList[index]->setText("Math "+Scale+" ");
    }else{
        channLabelList[index]->setText("CH"+QString::number(ch)+" "+Scale+" ");
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
        foreach(QLabel *label, measLabelList){
            label->hide();
        }
    }else{
        ui->widget_meas->show();
    }

    foreach(Measurement* m, meas){
        measLabelList.at(fillIndex)->setText(m->getLabel()+" "+m->getValueString());
        measLabelList.at(fillIndex)->show();
        measLabelList.at(fillIndex)->setStyleSheet("color:"+Colors::getChannelColor(m->getChannelIndex()));
        fillIndex++;
    }
}



void WidgetLabelArea::setCursorVoltageReadings(qreal curA, qreal curB)
{
    ui->label_Vcursor_title->setText("Cursors:");
    ui->label_Vcursor1->setText("A: "+ LabelFormator::formatOutout(curA,"V"));
    ui->label_Vcursor2->setText("B: "+ LabelFormator::formatOutout(curB,"V"));
    ui->label_Vcursor3->setText("Diff: "+ LabelFormator::formatOutout(curA-curB,"V"));
    ui->widget_Vcursors->show();
}

void WidgetLabelArea::setCursorTimeReadings(qreal curA, qreal curB)
{
    ui->label_Hcursor_title->setText("A: "+ LabelFormator::formatOutout(curA,"s"));
    ui->label_Hcursor1->setText("B: "+ LabelFormator::formatOutout(curB,"s"));
    ui->label_Hcursor2->setText("Diff: "+ LabelFormator::formatOutout(curA-curB,"s"));
    ui->label_Hcursor3->setText("Freq: "+ LabelFormator::formatOutout(abs(1/(curA-curB)),"Hz"));
    ui->widget_Hcursors->show();
}

void WidgetLabelArea::setCursorReadingsColor(int channelIndex)
{
    ui->widget_Hcursors->setStyleSheet("color:"+Colors::getChannelColor(channelIndex));
    ui->widget_Vcursors->setStyleSheet("color:"+Colors::getChannelColor(channelIndex));
}

void WidgetLabelArea::hideCursorReadings()
{
    ui->widget_Vcursors->hide();
    ui->widget_Hcursors->hide();
}

