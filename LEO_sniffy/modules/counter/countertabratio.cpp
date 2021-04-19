#include "countertabratio.h"

CounterTabRatio::CounterTabRatio(QVBoxLayout *destination, QWidget *parent) : QObject(parent)
{
    WidgetSeparator *separatorRetrigButton = new WidgetSeparator(parent,"Sampling retrigger");
    destination->addWidget(separatorRetrigButton);

    buttonStart = new WidgetButtons(parent, 1, ButtonTypes::NORMAL, 0);
    buttonStart->setObjectName("retrigger");
    buttonStart->setText("Start", 0);
    destination->addWidget(buttonStart);

    WidgetSeparator *separatorSampleCounte = new WidgetSeparator(parent,"Sampling of the reference input");
    destination->addWidget(separatorSampleCounte);

    dialSampleCount = new WidgetDialRange(parent, "Sample Count");
    dialSampleCount->setObjectName("dialRatSampleCount");
    dialSampleCount->setRange(2, REFERENCE_COUNT_MAX, "Smpls", 1, 1, REFERENCE_COUNT_DEFAULT, true, 0);
    dialSampleCount->setColor(Graphics::COLOR_CONTROLS);
    dialSampleCount->hideUnitSelection();
    destination->addWidget(dialSampleCount);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    destination->addItem(verticalSpacer);
}

void CounterTabRatio::setStartButton(bool start){
    if(start){
        buttonStart->setColor(Graphics::COLOR_RUNNING,0);
        buttonStart->setText("Stop",0);        
    }else{
        buttonStart->setColor(Graphics::COLOR_CONTROLS,0);
        buttonStart->setText("Start",0);
    }
}
