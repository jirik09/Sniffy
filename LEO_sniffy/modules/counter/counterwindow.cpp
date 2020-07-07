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

    WidgetSeparator *sepa_0 = new WidgetSeparator();
    ui->verticalLayout_display->addWidget(sepa_0);

    WidgetDisplay *frequencyDisplay = new WidgetDisplay("Frequency", "Hz", false);
    ui->verticalLayout_display->addWidget(frequencyDisplay);
    WidgetSeparator *sepa_1 = new WidgetSeparator();
    ui->verticalLayout_display->addWidget(sepa_1);

    WidgetDisplay *avgFrequencyDisplay = new WidgetDisplay("Average frequency", "Hz", true);
    ui->verticalLayout_display->addWidget(avgFrequencyDisplay);   
    WidgetSeparator *sepa_2 = new WidgetSeparator();
    ui->verticalLayout_display->addWidget(sepa_2);

    WidgetDisplay *avgPeriodDisplay = new WidgetDisplay("Average period", "s", true);
    ui->verticalLayout_display->addWidget(avgPeriodDisplay);
    WidgetSeparator *sepa_3 = new WidgetSeparator();
    ui->verticalLayout_display->addWidget(sepa_3);
}


CounterWindow::~CounterWindow()
{
    delete ui;
}
