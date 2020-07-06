#include "counterwindow.h"
#include "ui_windowcounter.h"

CounterWindow::CounterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);

    //********************* insert top options *********************
    widgetTab *tabs = new widgetTab(ui->widget_settings,4);
    ui->verticalLayout_settings->addWidget(tabs);
    tabs->setText("High Frequency",0);
    tabs->setText("Low Frequency",1);
    tabs->setText("Frequency Ratio",2);
    tabs->setText("Time Interval",3);

    WidgetDisplay *frequencyDisplay = new WidgetDisplay(false, "Hz", "Frequency", "", "Pins");
    ui->verticalLayout_display->addWidget(frequencyDisplay);
    WidgetDisplay *avgFrequencyDisplay = new WidgetDisplay(false, "Hz", "Average frequency", "", "");
    ui->verticalLayout_display->addWidget(avgFrequencyDisplay);
    WidgetDisplay *avgPeriodDisplay = new WidgetDisplay(false, "s", "Average period", "", "");
    ui->verticalLayout_display->addWidget(avgPeriodDisplay);
}


CounterWindow::~CounterWindow()
{
    delete ui;
}
