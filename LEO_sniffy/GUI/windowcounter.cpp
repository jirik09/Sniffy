/*
lol
*/

#include "windowcounter.h"
#include "ui_windowcounter.h"

WindowCounter::WindowCounter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindowCounter)
{
    ui->setupUi(this);

    WidgetTopSelection = new WidgetTop(ui->widget_left,4);
    WidgetTopSelection->setText("High Frequency",0);
    WidgetTopSelection->setText("Low Frequency",1);
    WidgetTopSelection->setText("Frequency Ratio",2);
    WidgetTopSelection->setText("Events Time Interval",3);
    WidgetTopSelection->setSelected(0);

    ui->verticalLayout_left->addWidget(WidgetTopSelection);

    //link options from topWidget
    connect(WidgetTopSelection->getPushButton(0),SIGNAL(clicked()),this,SLOT(highFrequencyClicked()));
    connect(WidgetTopSelection->getPushButton(1),SIGNAL(clicked()),this,SLOT(lowFrequencyClicked()));
    connect(WidgetTopSelection->getPushButton(2),SIGNAL(clicked()),this,SLOT(frequencyRatioClicked()));
    connect(WidgetTopSelection->getPushButton(3),SIGNAL(clicked()),this,SLOT(timeIntervalClicked()));

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);  //horizontal , vertical


    //************************* creating widget HF settings *******************

    //create scroll area - each seting selection has its own scroll area and other stuff
    scrollAreaHighFrequency = new QScrollArea(ui->widget_settings);
    scrollAreaHighFrequency->setWidgetResizable(true);
    scrollAreaHighFrequency->setSizePolicy(sizePolicy);

    WidgetHighFrequency = new QWidget();
    WidgetHighFrequency->setSizePolicy(sizePolicy);
    verticalLayoutHighFrequency = new QVBoxLayout(WidgetHighFrequency);

    scrollAreaHighFrequency->setContentsMargins(0,0,0,0);
    verticalLayoutHighFrequency->setSpacing(3);
    verticalLayoutHighFrequency->setContentsMargins(10,0,10,0);

    scrollAreaHighFrequency->setWidget(WidgetHighFrequency);
    ui->verticalLayout_settings->addWidget(scrollAreaHighFrequency);

    //add items - only specified widgets like buttons, dials, labels, separators....
    WidgetSeparator *separatorAveraging = new WidgetSeparator(WidgetHighFrequency,"Averaging");
    verticalLayoutHighFrequency->addWidget(separatorAveraging);

    WidgetDialRange *dialAvrg = new WidgetDialRange(WidgetHighFrequency ,"Averaging");
    dialAvrg->setRange(0,200," ",1,1,50);
    dialAvrg->hideUnitSelection();
    verticalLayoutHighFrequency->addWidget(dialAvrg);

    WidgetSeparator *separatorHorizontal = new WidgetSeparator(WidgetHighFrequency,"Other control components");
    verticalLayoutHighFrequency->addWidget(separatorHorizontal);

    // Separator at the end is very important otherwise controls would not be nicely shown when maximized
    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutHighFrequency->addItem(verticalSpacer);

    scrollAreaHighFrequency->show();


    //************************* creating widget LF settings *******************
    //create scroll area - each seting selection has its own scroll area
    scrollAreaLowFrequency = new QScrollArea(ui->widget_settings);
    scrollAreaLowFrequency->setWidgetResizable(true);
    scrollAreaLowFrequency->setSizePolicy(sizePolicy);

    WidgetLowFrequency = new QWidget();
    WidgetLowFrequency->setSizePolicy(sizePolicy);
    verticalLayoutLowFrequency = new QVBoxLayout(WidgetLowFrequency);

    scrollAreaLowFrequency->setContentsMargins(0,0,0,0);
    verticalLayoutLowFrequency->setSpacing(3);
    verticalLayoutLowFrequency->setContentsMargins(10,0,10,0);

    scrollAreaLowFrequency->setWidget(WidgetLowFrequency);
    ui->verticalLayout_settings->addWidget(scrollAreaLowFrequency);

    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayoutLowFrequency->addItem(verticalSpacer);

    scrollAreaLowFrequency->hide();
}


WindowCounter::~WindowCounter()
{
    delete ui;
}


void WindowCounter::highFrequencyClicked(){
    WidgetTopSelection->setSelected(0);
    scrollAreaHighFrequency->show();
    scrollAreaLowFrequency->hide();
}

void WindowCounter::lowFrequencyClicked(){
    WidgetTopSelection->setSelected(1);
    scrollAreaHighFrequency->hide();
    scrollAreaLowFrequency->show();
}

void WindowCounter::frequencyRatioClicked(){
    WidgetTopSelection->setSelected(2);
    scrollAreaHighFrequency->hide();
    scrollAreaLowFrequency->show();
}

void WindowCounter::timeIntervalClicked(){
    WidgetTopSelection->setSelected(3);
    scrollAreaHighFrequency->hide();
    scrollAreaLowFrequency->show();
}
