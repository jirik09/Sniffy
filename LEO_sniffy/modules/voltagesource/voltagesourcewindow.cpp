#include "voltagesourcewindow.h"
#include "ui_voltagesourcewindow.h"

VoltageSourceWindow::VoltageSourceWindow(VoltageSourceConfig *config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoltageSourceWindow),
    config(config)
{
    ui->setupUi(this);

    QString styleSheet = "image: url("+Graphics::getGraphicsPath()+"units_v.png); border: none;";
    WidgetDisplay *display;

    for (int i = 0 ;i<MAX_VOLTAGE_SOURCE_CHANNELS ;i++ ) {
        display  = new WidgetDisplay("Voltmeter_CH"+QString::number(i+1), "Channel "+QString::number(i+1), styleSheet, true, 1, HISTORY_SIZE, this);
        display->configLabel(0,"CH"+QString::number(i+1),Graphics::getChannelColor(i),true);
        displays.append(display);
        displays.at(i)->hide();
    }

    int index = 0;
    foreach(WidgetDisplay * dis, displays){
        dis->setContentsMargins(5, 5, 5, 5);
        dis->showAvgDisplay(false);
        dis->configLabel(2,"Voltage Source",Graphics::COLOR_TEXT_LABEL,true);
        ui->verticalLayout_display->addWidget(dis);
        dis->setProgressBarColor(Graphics::getChannelColor(index));
        index++;
        dis->showErrDisplay(false);
        dis->showTerrStyle(false);
    }

    QScrollArea *scrl = new QScrollArea(this);
    scrl->setWidgetResizable(true);
    scrl->setMinimumWidth(150);
    ui->verticalLayout_settings->addWidget(scrl);
    QWidget *setScroll = new QWidget(parent);

    scrl->setWidget(setScroll);
    QVBoxLayout *verChanBox = new QVBoxLayout();
    setScroll->setLayout(verChanBox);

    labelVDDA = new WidgetLabel(this,"Real Vcc","--");
    verChanBox->addWidget(labelVDDA);

    WidgetDialRange *dial;

    for (int i = 0 ;i<MAX_VOLTAGE_SOURCE_CHANNELS ;i++ ) {
        dial = new WidgetDialRange(this,"Voltage CH"+QString::number(i+1),i);
        dial->setObjectName("voltSourceDialCH"+QString::number(i+1));
        dial->setRange(0,1,"V",0.05,0.01,1000,false,3);
        dial->setColor(Graphics::getChannelColor(i));
        dial->hideUnitSelection();
        dials.append(dial);
        verChanBox->addWidget(dials.at(i));
        dials.at(i)->hide();
        connect(dials.at(i),&WidgetDialRange::valueChanged,this,&VoltageSourceWindow::dialChangedCallback);
    }

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacerLog;
    verticalSpacerLog = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verChanBox->addItem(verticalSpacerLog);

}

VoltageSourceWindow::~VoltageSourceWindow()
{
    delete ui;
}


void VoltageSourceWindow::restoreGUIAfterStartup()
{
    for (int i = 0 ;i<MAX_VOLTAGE_SOURCE_CHANNELS ;i++ ) {
        dialChangedCallback(dials.at(i)->getRealValue(),i);
    }
}

void VoltageSourceWindow::setNumberOfChannels(int numChannels)
{
    if(numChannels>MAX_VOLTAGE_SOURCE_CHANNELS) numChannels = MAX_VOLTAGE_SOURCE_CHANNELS;

    for (int i = 0; i<numChannels;i++ ) {
        dials.at(i)->show();
        displays.at(i)->show();
    }
}

void VoltageSourceWindow::setDisplayValue(qreal value, int channelIndex)
{
    displays.at(channelIndex)->displayString(displays.at(channelIndex)->formatNumber(value,'f',4));

}

void VoltageSourceWindow::setBarValue(qreal value, int channelIndex)
{
       displays.at(channelIndex)->updateProgressBar(value);
}

void VoltageSourceWindow::setRange(qreal min, qreal max)
{
    for (int i = 0 ;i<MAX_VOLTAGE_SOURCE_CHANNELS ;i++ ) {
        dials.at(i)->updateRange(min,max,true);
    }
}

void VoltageSourceWindow::setPins(QString pins[], int numChann)
{
    for(int i = 0;i<numChann;i++){
        displays.at(i)->configLabel(1,"pin "+pins[i],Graphics::COLOR_TEXT_ALL,true);
    }
}

void VoltageSourceWindow::setRealVdda(qreal value)
{
    labelVDDA->setValue(LabelFormator::formatOutout(value,"V",3));
}

void VoltageSourceWindow::dialChangedCallback(qreal value, int channel)
{
    emit voltageChanged(value, channel);
}
