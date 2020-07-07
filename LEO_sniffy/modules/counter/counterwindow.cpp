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

//    WidgetSeparator *sepa_0 = new WidgetSeparator();
//    ui->verticalLayout_display->addWidget(sepa_0);
//    sepa_0->layout()->setMargin(0);
//    sepa_0->hideLine();
//    sepa_0->resize(160, 5);

    WidgetDisplay *frequencyDisplay = new WidgetDisplay("Frequency", "Hz", false);
    ui->verticalLayout_display->addWidget(frequencyDisplay);
    frequencyDisplay->setContentsMargins(0, 6, 0, 0);

    WidgetDisplay *avgFrequencyDisplay = new WidgetDisplay("Average Frequency", "Hz", false);
    ui->verticalLayout_display->addWidget(avgFrequencyDisplay);   
    avgFrequencyDisplay->setContentsMargins(0, 3, 0, 3);

    WidgetDisplay *avgPeriodDisplay = new WidgetDisplay("Average Period", "s", false);
    ui->verticalLayout_display->addWidget(avgPeriodDisplay);
    avgPeriodDisplay->setContentsMargins(0, 0, 0, 6);
//    WidgetSeparator *sepa_3 = new WidgetSeparator();
//    ui->verticalLayout_display->addWidget(sepa_3);
//    sepa_3->layout()->setMargin(0);
//    sepa_3->hideLine();
}


CounterWindow::~CounterWindow()
{
    delete ui;
}
