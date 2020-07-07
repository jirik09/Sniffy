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

    WidgetDisplay *frequencyDisplay = new WidgetDisplay("Frequency", "Hz", true);
    ui->verticalLayout_display->addWidget(frequencyDisplay);
    frequencyDisplay->setContentsMargins(0, 6, 0, 0);
    frequencyDisplay->displayNumber("162 521 255.050410");

    WidgetDisplay *avgFrequencyDisplay = new WidgetDisplay("Average Frequency", "Hz", false);
    ui->verticalLayout_display->addWidget(avgFrequencyDisplay);   
    avgFrequencyDisplay->setContentsMargins(0, 3, 0, 3);

    WidgetDisplay *avgPeriodDisplay = new WidgetDisplay("Average Period", "s", false);
    ui->verticalLayout_display->addWidget(avgPeriodDisplay);
    avgPeriodDisplay->setContentsMargins(0, 0, 0, 6);


}


CounterWindow::~CounterWindow()
{
    delete ui;
}
