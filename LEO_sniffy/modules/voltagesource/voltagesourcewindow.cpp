#include "voltagesourcewindow.h"
#include "ui_voltagesourcewindow.h"
// <cstdio> removed after debug cleanup

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

    rangeMins.fill(0.0, MAX_VOLTAGE_SOURCE_CHANNELS);
    rangeMaxs.fill(3.3, MAX_VOLTAGE_SOURCE_CHANNELS); // default full scale 0..3.3 V
    int index = 0;
    for (WidgetDisplay *dis : displays) {
        // Set contents margins for 1st 0,5,5,5 and others 0,0,5,5
        if(index == 0) {
            dis->setContentsMargins(0, 5, 5, 5);
        } else {
            dis->setContentsMargins(0, 0, 5, 5);
        }
        dis->showAvgDisplay(false);
        dis->configLabel(2,"Voltage Source",Graphics::palette().textLabel,true);
        // Use vertical progress bar like Voltmeter
        dis->useVerticalProgressBar(true);
        // Initial default range 0..100 (we will remap when real range set)
        dis->setProgressRange(0,100);
        ui->verticalLayout_display->addWidget(dis);
        dis->setProgressColor(Graphics::getChannelColor(index));
        // Start at 0% explicitly
        dis->setProgressValue(0);
        index++;
        dis->showErrDisplay(false);
        dis->showTerrStyle(false);
        dis->hideHistoryPanel();
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
        // Set initial dial range to 0..3.3 V
        dial->setRange(0,3.3,"V",0.05,0.01,1000,false,3);
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
    if(channelIndex < 0 || channelIndex >= displays.size()) return;
    if(channelIndex >= dials.size()) return;
    qreal min = dials.at(channelIndex)->getRangeMin();
    qreal max = dials.at(channelIndex)->getRangeMax();
    if(max <= min){
        displays.at(channelIndex)->setProgressValue(0);
        return;
    }
    qreal ratio = (value - min) / (max - min);
    if(ratio < 0) ratio = 0; else if(ratio > 1) ratio = 1;
    int pctInt = static_cast<int>(ratio * 100.0 + 0.5);
    if(pctInt < 0) pctInt = 0; else if(pctInt > 100) pctInt = 100;
    displays.at(channelIndex)->setProgressValue(pctInt);
}

void VoltageSourceWindow::setRange(qreal min, qreal max)
{
    for (int i = 0 ;i<MAX_VOLTAGE_SOURCE_CHANNELS ;i++ ) {
        dials.at(i)->updateRange(min,max,true);
        // ensure all displays represent 0..100 percentage of voltage span
        displays.at(i)->setProgressRange(0,100);
        rangeMins[i] = min;
        rangeMaxs[i] = max;
        // Recalculate bar immediately using current dial value
        setBarValue(dials.at(i)->getRealValue(), i);
    }
}

void VoltageSourceWindow::setPins(QString pins[], int numChann)
{
    for(int i = 0;i<numChann;i++){
    displays.at(i)->configLabel(1,"pin "+pins[i],Graphics::palette().textAll,true);
    }
}

void VoltageSourceWindow::setRealVdda(qreal value)
{
    labelVDDA->setValue(LabelFormator::formatOutout(value,"V",3));
}

void VoltageSourceWindow::dialChangedCallback(qreal value, int channel)
{
    // Update bar percent immediately for visual feedback
    setBarValue(value, channel);
    emit voltageChanged(value, channel);
}
